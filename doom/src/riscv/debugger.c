#include "config.h"
#include "console.h"
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include "debugger.h"

const char hexdigits[] = "0123456789ABCDEF";
uint32_t dbg_current_thread = 0;

void putDebugChar(int dbgchar)
{
    IO_UARTTX[0] = (uint8_t)dbgchar;
}

int getDebugChar()
{
    while (IO_UARTRXByteCount == 0) { }
    return IO_UARTRX[0];
}

void exceptionHandler()
{
    // NOTE: This will be embedded in the core exception handler
}

void flush_i_cache()
{
    // TODO
}

// void *memset(void *, int, int) also in stdlib

// ----------------------------------------------------------

char packetbuffer[1200]; // Report 1024 but allocate more space just in case
int packetcursor = 0;
int checksumcounter = 0;
int packetcomplete = 0;

void processgdbpacket(char incoming)
{
    if (checksumcounter != 0)
    {
        packetbuffer[packetcursor++] = incoming;
        checksumcounter--;
        if (checksumcounter == 0)
        {
            packetcomplete = 1;
            packetbuffer[packetcursor] = 0;
            packetcursor = 0;
        }
    }
    else
    {
        if (incoming == '$')
        {
            packetcursor = 0;
            packetcomplete = 0;
        }
        else if (incoming == '#')
        {
            packetbuffer[packetcursor++] = incoming;
            checksumcounter = 2;
        }
        else
            packetbuffer[packetcursor++] = incoming;
    }
}

int startswith(char *source, const char *substr, int len)
{
    int cmpcnt = 0;

    while ((substr[cmpcnt] != 0) && (source[cmpcnt] == substr[cmpcnt]))
        ++cmpcnt;
    
    return (cmpcnt == len) ? 1 : 0;
}

void strchecksum(const char *str, char *checksumstr)
{
    int checksum = 0;
    int i=0;
    while(str[i]!=0)
    {
        checksum += str[i];
        ++i;
    }
    checksum = checksum%256;

    checksumstr[0] = hexdigits[((checksum>>4)%16)];
    checksumstr[1] = hexdigits[(checksum%16)];
    checksumstr[2] = 0;
}

void int2hex(const uint32_t val, char *regstring)
{
    regstring[0] = hexdigits[((val>>28)%16)];
    regstring[1] = hexdigits[((val>>24)%16)];
    regstring[2] = hexdigits[((val>>20)%16)];
    regstring[3] = hexdigits[((val>>16)%16)];
    regstring[4] = hexdigits[((val>>12)%16)];
    regstring[5] = hexdigits[((val>>8)%16)];
    regstring[6] = hexdigits[((val>>4)%16)];
    regstring[7] = hexdigits[(val%16)];
    regstring[8] = 0;
}

void uint2dec(const uint32_t val, char *msg)
{
    const char digits[] = "0123456789";

    int d = 1000000000;
    uint32_t enableappend = 0;
    uint32_t m = 0;
    /*if (i<0)
    msg[m++] = '-';*/
    for (int c=0;c<10;++c)
    {
        //uint32_t r = abs(val/d)%10;
        uint32_t r = (val/d)%10;
        // Ignore preceeding zeros
        if ((r!=0) || (enableappend) || (d==1))
        {
            enableappend = 1; // Rest of the digits can be appended
            msg[m++] = digits[r];
        }
        d = d/10;
    }
    msg[m] = 0;
}


void int2architectureorderedstring(const uint32_t val, char *regstring)
{
    regstring[6] = hexdigits[((val>>28)%16)];
    regstring[7] = hexdigits[((val>>24)%16)];
    regstring[4] = hexdigits[((val>>20)%16)];
    regstring[5] = hexdigits[((val>>16)%16)];
    regstring[2] = hexdigits[((val>>12)%16)];
    regstring[3] = hexdigits[((val>>8)%16)];
    regstring[0] = hexdigits[((val>>4)%16)];
    regstring[1] = hexdigits[(val%16)];
    regstring[8] = 0;
}

void byte2architectureorderedstring(const uint8_t val, char *regstring)
{
    regstring[0] = hexdigits[((val>>4)%16)];
    regstring[1] = hexdigits[(val%16)];
    regstring[2] = 0;
}

// NOTE: This cannot be re-entrant
void SendDebugPacketRegisters(struct cpu_context *task)
{
    char packetString[512];

    // All registers first
    for(uint32_t i=0;i<32;++i)
        int2architectureorderedstring(task->reg[i], &packetString[i*8]);

    // PC is sent last
    int2architectureorderedstring(task->PC, &packetString[32*8]);

    // Not sure if float registers follow?

    char checksumstr[3];
    strchecksum(packetString, checksumstr);

    printf("+$%s#%s",packetString,checksumstr);
}

// NOTE: This cannot be re-entrant
void SendDebugPacket(const char *packetString)
{
    char checksumstr[3];
    strchecksum(packetString, checksumstr);

    printf("+$%s#%s",packetString,checksumstr);
}

uint32_t dec2int(char *dec)
{
    uint32_t val = 0;
    while (*dec) {
        uint8_t digit = *dec++;
        if (digit >= '0' && digit <= '9')
            digit = digit - '0';
        val = (val*10) + digit;
    }
    return val;
}

uint32_t hex2int(char *hex)
{
    uint32_t val = 0;
    while (*hex) {
        uint8_t nibble = *hex++;
        if (nibble >= '0' && nibble <= '9')
            nibble = nibble - '0';
        else if (nibble >= 'a' && nibble <='f')
            nibble = nibble - 'a' + 10;
        else if (nibble >= 'A' && nibble <='F')
            nibble = nibble - 'A' + 10;
        val = (val << 4) | (nibble & 0xF);
    }
    return val;
}

void RemoveBreakPoint(struct cpu_context *task, uint32_t breakaddress)
{
    for (uint32_t i=0; i<task->num_breakpoints; ++i)
    {
        if (task->breakpoints[i].address == breakaddress)
        {
            // Restore saved instruction
            *(uint32_t*)(breakaddress) = task->breakpoints[i].originalinstruction;
            
            if (task->num_breakpoints-1 != i)
            {
                task->breakpoints[i].originalinstruction = task->breakpoints[task->num_breakpoints-1].originalinstruction;
                task->breakpoints[i].address = task->breakpoints[task->num_breakpoints-1].address;
            }

            task->num_breakpoints--;
            //printf("RMV 0x%.8X\r\n", breakaddress);
        }
    }
}

void AddBreakPoint(struct cpu_context *task, uint32_t breakaddress)
{
    task->breakpoints[task->num_breakpoints].address = breakaddress;
    // Save instruction
    task->breakpoints[task->num_breakpoints].originalinstruction = *(uint32_t*)(breakaddress);
    task->num_breakpoints++;

    // Replace with EBREAK
    *(uint32_t*)(breakaddress) = 0x00100073;

    //printf("BRK 0x%.8X\r\n", breakaddress);
}

uint32_t gdb_breakpoint(struct cpu_context tasks[])
{
    if (tasks[dbg_current_thread].breakhit == 0)
    {
        tasks[dbg_current_thread].breakhit = 1;
        // https://man7.org/linux/man-pages/man7/signal.7.html
        // https://chromium.googlesource.com/native_client/nacl-gdb/+/refs/heads/main/include/gdb/signals.def
        //SendDebugPacket("S05"); // S05==SIGTRAP, S02==SIGINT 
        SendDebugPacket("T02"); // T0X can also send the 'important' registers and their values across such as PC/RA/SP, but S0X  cannot
        return 0x1;
    }

    return 0x0;
}

uint32_t gdb_handler(struct cpu_context tasks[], const uint32_t num_tasks)
{
    // NOTES:
    // Checksum is computed as the modulo 256 sum of the packet info characters.
    // For each packet received, responses is either + (for accepted) or - (for retry): $packet-data#checksum
    // Packets start with $, end with # and a two-digit hex checksum
    // GDB has a cache of registers

    // Some Commands:
    // g: request value of CPU registers
    // G: set value of CPU registers
    // maddr,count: read count bytes at addr
    // Maddr,count:... write count bytes at addr
    // c c [addr] resume execution at current PC or at addr if supplied
    // s s [addr] step the program one insruction from current PC or addr if supplied
    // k kill the target program
    // ? report the most recent signal
    // T allows remote to send only the registers required for quick decisions (step/conditional breakpoints)

    char outstring[1024] = "";

    int external_break = 0;
    while (*IO_UARTRXByteCount)
    {
        char checkchar = *IO_UARTRX;

        if (checkchar == '\003')
            external_break = 1;
        processgdbpacket(checkchar);
    }

    if (external_break)
        tasks[dbg_current_thread].ctrlc = 1; // Stop on first chance

    // ACK
    if (packetcomplete)
    {
        packetcomplete = 0;

        // qSupported: respond with 'hwbreak; swbreak'

        if (startswith(packetbuffer, "qSupported", 10))
            SendDebugPacket("swbreak+;hwbreak+;multiprocess-;qXfer:threads:read+;PacketSize=1024");
        else if (startswith(packetbuffer, "qXfer:threads:read::", 20))
        {
            char offsetbuf[12];//, lenbuf[12];
            int a=0, p=20;
            while (packetbuffer[p]!=',')
                offsetbuf[a++] = packetbuffer[p++];
            offsetbuf[a]=0;
            ++p; // skip the comma
            /*
            int c=0;
            while (packetbuffer[p]!=':')
                lenbuf[c++] = packetbuffer[p++];
            lenbuf[c]=0;*/

            uint32_t offset = dec2int(offsetbuf);
            //uint32_t length = dec2int(lenbuf);

            if (offset == 0)
            {
                strcat(outstring, "l<?xml version=\"1.0\"?>\n<threads>\n");
                for (uint32_t i=0;i<num_tasks;++i)
                {
                    char threadid[10];
                    uint2dec(i+1, threadid);
                    strcat(outstring, "<thread id=\"");
                    strcat(outstring, threadid);
                    strcat(outstring, "\" core=\"0\" name=\"");
                    strcat(outstring, tasks[i].name);
                    strcat(outstring, "\"></thread>\n");
                }
                strcat(outstring, "</threads>\n");
                SendDebugPacket(outstring);
            }
            else
                SendDebugPacket(""); // Not sure what to send here
        }
        /*else if (startswith(packetbuffer, "qXfer:traceframe-info:read::", 28))
        {
            // offset, length
            <?xml version="1.0"?>
            <traceframe-info>
            <memory start="" length="">
            <tvar id="">
            </traceframe-info>
            SendDebugPacket(""); // TBD
        }*/
        else if (startswith(packetbuffer, "vMustReplyEmpty", 15))
            SendDebugPacket(""); // Have to reply empty
        else if (startswith(packetbuffer, "qTStatus", 8))
            SendDebugPacket(""); // Not supported
        else if (startswith(packetbuffer, "?", 1))
        {
            tasks[dbg_current_thread].ctrlc = 1; // Stop on first chance
        }
        else if (startswith(packetbuffer, "qfThreadInfo", 12))
            SendDebugPacket("l"); // Not supported
        else if (startswith(packetbuffer, "qSymbol", 7))
            SendDebugPacket("OK"); // No symtable info required
        else if (startswith(packetbuffer, "qAttached", 9))
            SendDebugPacket("1");
        else if (startswith(packetbuffer, "qOffsets", 8))
            SendDebugPacket("Text=0;Data=0;Bss=0"); // No relocation
        else if (startswith(packetbuffer, "qRcmd,", 6))
        {
            SendDebugPacket("OK"); // ?
        }
        else if (startswith(packetbuffer, "vCont?", 6))
        {
            SendDebugPacket("vCont;c;s;t"); // Continue/step/stop actions supported
        }
        /*else if (startswith(packetbuffer, "vCont", 5))
        {
            if (packetbuffer[5]=='c') // Continue action
                tasks[dbg_current_thread].ctrlc = 8;
            if (packetbuffer[5]=='s') // Step action
                EchoConsole("step\r\n");
            if (packetbuffer[5]=='t') // Stop action
                tasks[dbg_current_thread].ctrlc = 1;

            SendDebugPacket("OK");
        }*/
        else if (startswith(packetbuffer, "X1", 2)) // X1 addr, length : XX.. Write binary data to memory
        {
            SendDebugPacket(""); // Not applicable
        }
        else if (startswith(packetbuffer, "qL", 2))
            SendDebugPacket(""); // Not supported
        else if (startswith(packetbuffer, "qC", 2))
            SendDebugPacket(""); // Not supported
        else if (startswith(packetbuffer, "z0,", 3)) // remove breakpoint
        {
            uint32_t breakaddress;

            char hexbuf[12];
            int a=0,p=3;
            while (packetbuffer[p]!=',')
                hexbuf[a++] = packetbuffer[p++];
            hexbuf[a]=0;
            breakaddress = hex2int(hexbuf);

            RemoveBreakPoint(&tasks[dbg_current_thread], breakaddress);
            SendDebugPacket("OK");
        }
        else if (startswith(packetbuffer, "Z0,", 3)) // insert breakpoint
        {
            uint32_t breakaddress;

            char hexbuf[12];
            int a=0,p=3;
            while (packetbuffer[p]!=',')
                hexbuf[a++] = packetbuffer[p++];
            hexbuf[a]=0;
            breakaddress = hex2int(hexbuf); // KIND code is probably '4' after this (standard 32bit break)

            AddBreakPoint(&tasks[dbg_current_thread], breakaddress);
            SendDebugPacket("OK");
        }
        //else if (startswith(packetbuffer, "G", 1)) // Write registers
        else if (startswith(packetbuffer, "g", 1)) // List registers
        {
            SendDebugPacketRegisters(&tasks[dbg_current_thread]);
        }
        //else if (startswith(packetbuffer, "P", 1)) // Write register
        else if (startswith(packetbuffer, "p", 1)) // Print register, p??
        {
            char hexbuf[9], regstring[9];
            int r=0,p=1;
            while (packetbuffer[p]!='#')
                hexbuf[r++] = packetbuffer[p++];
            hexbuf[r]=0;
            r = hex2int(hexbuf);

            int2architectureorderedstring(tasks[dbg_current_thread].reg[r], regstring);

            SendDebugPacket(regstring); // Return register data
        }
        else if (startswith(packetbuffer, "D", 1)) // Detach
            SendDebugPacket("");
        else if (startswith(packetbuffer, "M", 1)) // Set memory, maddr,count:bytes
        {
            char addrbuf[12], cntbuf[12];
            int a=0,c=0, p=1;
            while (packetbuffer[p]!=',')
                addrbuf[a++] = packetbuffer[p++];
            addrbuf[a]=0;
            ++p; // skip the comma
            while (packetbuffer[p]!=':')
                cntbuf[c++] = packetbuffer[p++];
            cntbuf[c]=0;
            ++p; // skip the column

            uint32_t addrs = hex2int(addrbuf);
            uint32_t numbytes = dec2int(cntbuf);

            char bytebuf[3];
            bytebuf[2] = 0;
            for (uint32_t i=0; i<numbytes; ++i)
            {
                bytebuf[0] = packetbuffer[p++];
                bytebuf[1] = packetbuffer[p++];
                uint32_t byteval = hex2int(bytebuf);
                *(uint8_t*)(addrs+i) = (uint8_t)byteval;
            }
            SendDebugPacket("OK");
        }
        else if (startswith(packetbuffer, "m", 1)) // Read memory, maddr,count
        {
            char addrbuf[12], cntbuf[12];
            int a=0,c=0, p=1;
            while (packetbuffer[p]!=',')
                addrbuf[a++] = packetbuffer[p++];
            addrbuf[a]=0;
            ++p; // skip the comma
            while (packetbuffer[p]!='#')
                cntbuf[c++] = packetbuffer[p++];
            cntbuf[c]=0;

            uint32_t addrs = hex2int(addrbuf);
            uint32_t numbytes = dec2int(cntbuf);

            uint32_t ofst = 0;
            for (uint32_t i=0; i<numbytes; ++i)
            {
                uint8_t memval = *(uint8_t*)(addrs+i);
                byte2architectureorderedstring(memval, &outstring[ofst]);
                ofst += 2;
            }

            SendDebugPacket(outstring);
        }
        else if (startswith(packetbuffer, "s", 1)) // Step
        {
            //tasks[dbg_current_thread].ctrlc = 16;
            SendDebugPacket(""); // Not sure what to do here, doesn't seem to work
        }
        else if (startswith(packetbuffer, "T", 1)) // T threadid (find out if threadid is alive)
        {
            // TODO: Send E?? if thread is dead (does that also mean, breakpoint hit?)
            SendDebugPacket("OK");
        }
        else if (startswith(packetbuffer, "c", 1)) // Continue
        {
            tasks[dbg_current_thread].ctrlc = 8; // Supposed to be deprecated
            SendDebugPacket("OK");
        }
        else if (startswith(packetbuffer, "H", 1))
        {
            // m/M/g/G etc
            // NOTE: Thread id == -1 means 'all threads'
            if (packetbuffer[1]=='g') // Select thread
            {
                char threadbuf[12];
                int a=0, p=2;
                while (packetbuffer[p]!='#')
                    threadbuf[a++] = packetbuffer[p++];
                threadbuf[a]=0;
                uint32_t idx = hex2int(threadbuf);
                if (idx == 0) // Eh?
                    dbg_current_thread = 0;
                else
                    dbg_current_thread = idx-1; // Thread IDs start from one, so we have to go one down

                SendDebugPacket("OK");
            }
            if (packetbuffer[1]=='c') // Continue
            {
                tasks[dbg_current_thread].ctrlc = 8;
                SendDebugPacket("OK"); // deprecated, use vCont
            }
        }
        /*else // Unknown command
        {
            EchoConsole(">");
            EchoConsole(packetbuffer); // NOTE: Don't do this
            EchoConsole("\r\n");
        }*/
    }

    return 0x0; // TODO: might want to pass data back
}
