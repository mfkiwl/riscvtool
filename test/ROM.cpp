// Bootloader

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include "rvcrt0.h"

volatile unsigned char* VRAM = (volatile unsigned char* )0x80000000;       // Video Output: VRAM starts at 0, continues for 0xC000 bytes (256x192 8 bit packed color pixels, RGB[3:3:2] format)
volatile unsigned int* UARTRXStatus = (volatile unsigned int* )0x60000000; // UART input status (read)
volatile unsigned char* UARTTX = (volatile unsigned char* )0x40000000;     // UART send data (write)
volatile unsigned char* UARTRX = (volatile unsigned char* )0x50000000;     // UART receive data (read)
volatile unsigned int targetjumpaddress = 0x00000000;

unsigned int load()
{
   // Header data
   unsigned int loadlen = 0;
   unsigned int loadtarget = 0;
   char *loadlenaschar = (char*)&loadlen;
   char *loadtargetaschar = (char*)&loadtarget;

   // Data length
   unsigned int writecursor = 0;
   while(writecursor < 4)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         loadlenaschar[writecursor++] = readdata;
      }
   }

   // Target address
   writecursor = 0;
   while(writecursor < 4)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         loadtargetaschar[writecursor++] = readdata;
      }
   }

   // Read binary data
   writecursor = 0;
   volatile unsigned char* target = (volatile unsigned char* )loadtarget;
   while(writecursor < loadlen)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         target[writecursor++] = readdata;
      }
   }

   return loadtarget;
}

void cls()
{
   for(int y=0;y<192;++y)
   {
      int py = y<<8;
      for(int x=0;x<256;++x)
         VRAM[x+py] = 0x30;
   }
}

int main()
{
   // 32 bytes of incoming command space
   char incoming[32];

   unsigned int rcvcursor = 0;
   unsigned int oldcount = 0;

   cls();

   // UART communication section
   while(1)
   {
      // Step 1: Read UART FIFO byte count
      unsigned int bytecount = UARTRXStatus[0];

      // Step 2: Check to see if we have something in the FIFO
      if (bytecount != 0)
      {
         // Step 3: Read the data on UARTRX memory location
         char checkchar = incoming[rcvcursor++] = UARTRX[0];

         if (checkchar == 13) // Enter?
         {
            // Terminate the string
            incoming[rcvcursor-1] = 0;

            // Run the incoming binary
            //if (!strcmp(incoming, "run")
            if (incoming[0]='r' && incoming[1]=='u' && incoming[2]=='n')
            {
               targetjumpaddress = load();
                ((void (*)(void)) targetjumpaddress)();
            }

            // Rewind read cursor
            rcvcursor=0;
         }

         // Echo characters back to the terminal
         UARTTX[0] = checkchar;
         if (checkchar == 13)
            UARTTX[0] = 10; // Echo a linefeed

         if (rcvcursor>31)
            rcvcursor = 0;
      }
   }

   return 0;
}
