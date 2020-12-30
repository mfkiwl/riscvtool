#include <windows.h>
#include <stdio.h>
#include <chrono>
#include <thread>

#pragma pack(push,1)
struct SElfFileHeader32
{
    unsigned int m_Magic;       // Magic word (0x7F followed by 'ELF' -> 0x464C457F)
    unsigned char m_Class;
    unsigned char m_Data;
    unsigned char m_EI_Version;
    unsigned char m_OSABI;
    unsigned char m_ABIVersion;
    unsigned char m_Pad[7];
    unsigned short m_Type;      // NONE(0x00)/REL(0x01)/EXEC(0x02)/DYN(0x03)/CORE(0x04)/LOOS(0xFE00)/HIOS(0xFEFF)/LOPROC(0xFF00)/HIPROC(0xFFFF)
    unsigned short m_Machine;   // RISC-V: 0xF3
    unsigned int m_Version;
    unsigned int m_Entry;
    unsigned int m_PHOff;       // Program header offset in file
    unsigned int m_SHOff;
    unsigned int m_Flags;
    unsigned short m_EHSize;
    unsigned short m_PHEntSize;
    unsigned short m_PHNum;
    unsigned short m_SHEntSize;
    unsigned short m_SHNum;
    unsigned short m_SHStrndx;
};
struct SElfProgramHeader32
{
    unsigned int m_Type;
    unsigned int m_Offset;      // Offset of entry point
    unsigned int m_VAddr;
    unsigned int m_PAddr;
    unsigned int m_FileSz;
    unsigned int m_MemSz;       // Length of code in memory (includes code and data sections)
    unsigned int m_Flags;
    unsigned int m_Align;
};
#pragma pack(pop)

void parseelfheader(unsigned char *_elfbinary)
{
    // Parse the header and check the magic word
    SElfFileHeader32 *fheader = (SElfFileHeader32 *)_elfbinary;

    if (fheader->m_Magic != 0x464C457F)
    {
        printf("Unknown magic, expecting 0x7F followed by 'ELF', got '%c%c%c%c' (%.8X)\n", fheader->m_Magic&0x000000FF, (fheader->m_Magic&0x0000FF00)>>8, (fheader->m_Magic&0x00FF0000)>>16, (fheader->m_Magic&0xFF000000)>>24, fheader->m_Magic);
        return;
    }

    // Parse the header and dump the executable
    SElfProgramHeader32 *pheader = (SElfProgramHeader32 *)(_elfbinary+fheader->m_PHOff);

    printf("memory_initialization_radix=16;\nmemory_initialization_vector=\n");
    unsigned char *litteendian = (unsigned char *)(_elfbinary+pheader->m_Offset);
    for (unsigned int i=0;i<pheader->m_MemSz;++i)
    {
        if (i!=0 && ((i%16) == 0))
            printf("\n");
        printf("%.2X", litteendian[(i&0xFFFFFFFC) + 3-(i%4)]);
        if (((i+1)%4)==0 && i!=pheader->m_MemSz-1)
            printf(" ");
    }
    printf(";");
}

void dumpelf(char *_filename)
{
    HANDLE hComm;
    DCB serialParams{0};
    COMMTIMEOUTS timeouts{0};

    FILE *fp;
    fp = fopen(_filename, "rb");
    if (!fp)
    {
        printf("ERROR: can't open ELF file %s\n", _filename);
        return;
    }
	unsigned int filebytesize = 0;
	fpos_t pos, endpos;
	fgetpos(fp, &pos);
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &endpos);
	fsetpos(fp, &pos);
	filebytesize = (unsigned int)endpos;

    // TODO: Actual binary to send starts at 0x1000
    unsigned char *bytestosend = new unsigned char[filebytesize];
    fread(bytestosend, 1, filebytesize, fp);
    fclose(fp);

    parseelfheader(bytestosend);

    delete [] bytestosend;
}

void sendcommand(char *_commandstring)
{
    HANDLE hComm;
    DCB serialParams{0};
    COMMTIMEOUTS timeouts{0};

    int commandlength = strlen(_commandstring);

    printf("Sending command over COM4 @256000 bps...\n");
    hComm = CreateFileA("\\\\.\\COM4", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hComm != INVALID_HANDLE_VALUE)
    {
        serialParams.DCBlength = sizeof(serialParams);
        if (GetCommState(hComm, &serialParams))
        {
            serialParams.BaudRate = CBR_256000;
            serialParams.ByteSize = 8;
            serialParams.StopBits = ONESTOPBIT;
            serialParams.Parity = NOPARITY;

            if (SetCommState(hComm, &serialParams) != 0)
            {
                timeouts.ReadIntervalTimeout = 50;
                timeouts.ReadTotalTimeoutConstant = 50;
                timeouts.ReadTotalTimeoutMultiplier = 10;
                timeouts.WriteTotalTimeoutConstant = 50;
                timeouts.WriteTotalTimeoutMultiplier = 10;
                if (SetCommTimeouts(hComm, &timeouts) != 0)
                {
                    DWORD byteswritten;
                    // Send the command
                    WriteFile(hComm, _commandstring, commandlength, &byteswritten, nullptr);
                    // Plus, a return at the end so it gets executed
                    WriteFile(hComm, "\r", 1, &byteswritten, nullptr);
                    printf("Done\n");
                }
                else
                {
                    printf("ERROR: can't set comm timeouts\n");
                }
                
            }
            else
            {
                printf("ERROR: can't set comm parameters\n");
            }
        }
        else
        {
            printf("ERROR: can't get comm parameters\n");
        }
    }
    else
    {
        printf("ERROR: can't open comm on port COM4\n");
    }

    CloseHandle(hComm);
}

void sendbinary(char *_filename, const unsigned int _target=0x80000000)
{
    HANDLE hComm;
    DCB serialParams{0};
    COMMTIMEOUTS timeouts{0};

    FILE *fp;
    fp = fopen(_filename, "rb");
    if (!fp)
    {
        printf("ERROR: can't open binary file %s\n", _filename);
        return;
    }
	unsigned int filebytesize = 0;
    unsigned int targetaddress = _target;
	fpos_t pos, endpos;
	fgetpos(fp, &pos);
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &endpos);
	fsetpos(fp, &pos);
	filebytesize = (unsigned int)endpos;

    // TODO: Actual binary to send starts at 0x1000
    unsigned char *bytestosend = new unsigned char[filebytesize+8];
    fread(bytestosend+8, 1, filebytesize, fp);
    fclose(fp);
    ((unsigned int*)bytestosend)[0] = filebytesize;
    ((unsigned int*)bytestosend)[1] = targetaddress;

    char commandtosend[512];
    int commandlength=0;
    sprintf(commandtosend, "load%c", 13);
    commandlength = strlen(commandtosend);

    printf("Sending raw binary file over COM4 @256000 bps at 0x%.8X\n", _target);
    hComm = CreateFileA("\\\\.\\COM4", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hComm != INVALID_HANDLE_VALUE)
    {
        serialParams.DCBlength = sizeof(serialParams);
        if (GetCommState(hComm, &serialParams))
        {
            serialParams.BaudRate = CBR_256000;
            serialParams.ByteSize = 8;
            serialParams.StopBits = ONESTOPBIT;
            serialParams.Parity = NOPARITY;

            if (SetCommState(hComm, &serialParams) != 0)
            {
                timeouts.ReadIntervalTimeout = 50;
                timeouts.ReadTotalTimeoutConstant = 50;
                timeouts.ReadTotalTimeoutMultiplier = 10;
                timeouts.WriteTotalTimeoutConstant = 50;
                timeouts.WriteTotalTimeoutMultiplier = 10;
                if (SetCommTimeouts(hComm, &timeouts) != 0)
                {
                    DWORD byteswritten;
                    // Send the string "load\r"
                    WriteFile(hComm, commandtosend, commandlength, &byteswritten, nullptr);
                    // Wait a bit for the receiving end to start accepting
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    // Send raw binary with 8byte header (file size)
                    WriteFile(hComm, bytestosend, filebytesize+8, &byteswritten, nullptr);
                    // Wait a bit after sending last bytes to avoid issues with repeated calls
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    printf("Done\n");
                }
                else
                {
                    printf("ERROR: can't set comm timeouts\n");
                }
                
            }
            else
            {
                printf("ERROR: can't set comm parameters\n");
            }
        }
        else
        {
            printf("ERROR: can't get comm parameters\n");
        }
    }
    else
    {
        printf("ERROR: can't open comm on port COM4\n");
    }

    CloseHandle(hComm);
    delete [] bytestosend;
}


unsigned int generateelfuploadpackage(unsigned char *_elfbinary)
{
    // Parse the header and check the magic word
    SElfFileHeader32 *fheader = (SElfFileHeader32 *)_elfbinary;

    if (fheader->m_Magic != 0x464C457F)
    {
        printf("ERROR: Unknown magic, expecting 0x7F followed by 'ELF', got '%c%c%c%c' (%.8X)\n", fheader->m_Magic&0x000000FF, (fheader->m_Magic&0x0000FF00)>>8, (fheader->m_Magic&0x00FF0000)>>16, (fheader->m_Magic&0xFF000000)>>24, fheader->m_Magic);
        return 0xFFFFFFFF;
    }

    // Parse the header and dump the executable
    SElfProgramHeader32 *pheader = (SElfProgramHeader32 *)(_elfbinary+fheader->m_PHOff);

    // Aliasing to re-use same buffer
    unsigned int *targetarea = (unsigned int*)_elfbinary;
    unsigned int *sourcearea = (unsigned int*)(_elfbinary+pheader->m_Offset);
    unsigned int binarysize = pheader->m_MemSz;

    printf("Preparing %dbyte binary package (relocating executable code and data from 0x%.8X to 0x00000000)\n", pheader->m_MemSz, pheader->m_Offset);
    for (unsigned int i=0;i<binarysize/4;++i)
    {
        printf("src %.8X:%.8X\n", i, sourcearea[i]);
        targetarea[i] = sourcearea[i];
    }

    return binarysize;
}

void sendelf(char *_filename, const unsigned int _target=0x00000000)
{
    HANDLE hComm;
    DCB serialParams{0};
    COMMTIMEOUTS timeouts{0};

    FILE *fp;
    fp = fopen(_filename, "rb");
    if (!fp)
    {
        printf("ERROR: can't open ELF file %s\n", _filename);
        return;
    }
	unsigned int filebytesize = 0;
    unsigned int targetaddress = _target;
	fpos_t pos, endpos;
	fgetpos(fp, &pos);
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &endpos);
	fsetpos(fp, &pos);
	filebytesize = (unsigned int)endpos;

    // TODO: Actual binary to send starts at 0x1000
    unsigned char *bytestosend = new unsigned char[filebytesize+8];
    fread(bytestosend+8, 1, filebytesize, fp);
    fclose(fp);
    unsigned int actualbinarysize = generateelfuploadpackage(bytestosend+8);
    if (actualbinarysize == 0xFFFFFFFF) // Failed
    {
        printf("ERROR: header parse error, can't send ELF file %s\n", _filename);
        return;
    }
    else
        printf("Actual ELF executable portion size: 0x%.8X\n", actualbinarysize);
    ((unsigned int*)bytestosend)[0] = actualbinarysize;
    ((unsigned int*)bytestosend)[1] = targetaddress;

    char commandtosend[512];
    int commandlength=0;
    sprintf(commandtosend, "run%c", 13);
    commandlength = strlen(commandtosend);

    printf("Sending ELF binary executable portion over COM4 @256000 bps at 0x%.8X\n", _target);
    hComm = CreateFileA("\\\\.\\COM4", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hComm != INVALID_HANDLE_VALUE)
    {
        serialParams.DCBlength = sizeof(serialParams);
        if (GetCommState(hComm, &serialParams))
        {
            serialParams.BaudRate = CBR_256000;
            serialParams.ByteSize = 8;
            serialParams.StopBits = ONESTOPBIT;
            serialParams.Parity = NOPARITY;

            if (SetCommState(hComm, &serialParams) != 0)
            {
                timeouts.ReadIntervalTimeout = 50;
                timeouts.ReadTotalTimeoutConstant = 50;
                timeouts.ReadTotalTimeoutMultiplier = 10;
                timeouts.WriteTotalTimeoutConstant = 50;
                timeouts.WriteTotalTimeoutMultiplier = 10;
                if (SetCommTimeouts(hComm, &timeouts) != 0)
                {
                    DWORD byteswritten;
                    // Send the string "load\r"
                    WriteFile(hComm, commandtosend, commandlength, &byteswritten, nullptr);
                    // Wait a bit for the receiving end to start accepting
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    // Send raw binary with 8byte header (file size)
                    WriteFile(hComm, bytestosend, actualbinarysize+8, &byteswritten, nullptr);
                    // Wait a bit after sending last bytes to avoid issues with repeated calls
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    printf("Done\n");
                }
                else
                {
                    printf("ERROR: can't set comm timeouts\n");
                }
                
            }
            else
            {
                printf("ERROR: can't set comm parameters\n");
            }
        }
        else
        {
            printf("ERROR: can't get comm parameters\n");
        }
    }
    else
    {
        printf("ERROR: can't open comm on port COM4\n");
    }

    CloseHandle(hComm);
    delete [] bytestosend;
}

int main(int argc, char **argv)
{
    if (argc>1)
    {
        if (argc>2)
        {
            if (strstr(argv[2], "-makerom"))
            {
                dumpelf(argv[1]);
            }
            if (strstr(argv[2], "-sendelf"))
            {
                unsigned int target = (unsigned int)strtoul(argv[3], nullptr, 16);
                sendelf(argv[1], target);
            }
            if (strstr(argv[2], "-sendraw"))
            {
                unsigned int target = (unsigned int)strtoul(argv[3], nullptr, 16);
                sendbinary(argv[1], target);
            }
            if (strstr(argv[2], "-sendcommand"))
            {
                sendcommand(argv[1]);
            }
        }
        else
            sendbinary(argv[1]); // By default send as raw bitmap image to 0x80000000
    }
    else
    {
        printf("RISCVTool\n");
        printf("Usage: riscvtool.exe binaryfilename [-sendelf hexaddress|-sendraw hexaddress]\n");
        printf("NOTE: If no option is supplied, binary upload is assumed to target address 0x80000000\n");
    }
    
}
