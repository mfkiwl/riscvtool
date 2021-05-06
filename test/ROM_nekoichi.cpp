// Bootloader

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#define STARTUP_ROM
#include "rom_nekoichi_rvcrt0.h"

#include "utils.h"
#include "gpu.h"

void loadbinaryblob()
{
   // Header data
   unsigned int loadlen = 0;
   unsigned int loadtarget = 0;
   char *loadlenaschar = (char*)&loadlen;
   char *loadtargetaschar = (char*)&loadtarget;

   // Target address
   unsigned int writecursor = 0;
   while(writecursor < 4)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         loadtargetaschar[writecursor++] = readdata;
      }
   }

   // Data length
   writecursor = 0;
   while(writecursor < 4)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         loadlenaschar[writecursor++] = readdata;
      }
   }

   int x1 = 0;
   int y1 = 0;
   int x2 = 8;
   int y2 = 0;
   int x3 = 0;
   int y3 = 8;

   // Read binary blob
   writecursor = 0;
   volatile unsigned char* target = (volatile unsigned char* )loadtarget;
   uint8_t ncolor = 0;
   while(writecursor < loadlen)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         target[writecursor++] = readdata;

         // Draw a small colored triangle every time we receive a byte
         uint32_t vertex10 = ((y2&0xFF)<<24) | ((x2&0xFF)<<16) | ((y1&0xFF)<<8) | (x1&0xFF);
         uint32_t vertexC2 = ((ncolor&0xFF)<<16) | ((y3&0xFF)<<8) | (x3&0xFF);
         GPUFIFO[0] = GPUOPCODE(GPUSETREGISTER, 0, 2, GPU22BITIMM(vertex10)); // {v1.y, v1.x, v0.y, v0.x}
         GPUFIFO[1] = GPUOPCODE(GPUSETREGISTER, 2, 2, GPU10BITIMM(vertex10));
         GPUFIFO[2] = GPUOPCODE(GPUSETREGISTER, 0, 3, GPU22BITIMM(vertexC2)); // {0, frontcolor, v2.y, v2.x}
         GPUFIFO[3] = GPUOPCODE(GPUSETREGISTER, 3, 3, GPU10BITIMM(vertexC2));
         GPUFIFO[4] = GPUOPCODE(GPURASTERIZE, 2, 3, 0);

         x1 += 8; x2 += 8; x3 += 8;
         if (x1 > 248)
         {
            x1 = 0; x2 = 8; x3 = 0;
            y1 = y1+8; y2 = y2+8; y3 = y3+8;
            if (y1 > 184)
            {
               y1 = 0; y2 = 0; y3 = 8;
            }
         }
         ncolor+=readdata;
      }
   }
}

void runbinary()
{
   // Header data
   unsigned int branchaddress = 0;
   char *branchaddressaschar = (char*)&branchaddress;

   // Data length
   unsigned int writecursor = 0;
   while(writecursor < 4)
   {
      unsigned int bytecount = UARTRXStatus[0];
      if (bytecount != 0)
      {
         unsigned char readdata = UARTRX[0];
         branchaddressaschar[writecursor++] = readdata;
      }
   }

   // Jump to entry point
   ((void (*)(void)) branchaddress)();

   // Or alternatively, set 'ra' to branchaddress and 'ret' since we won't be exiting main()
}

void echoterm(const char *_message)
{
   int i=0;
   while (_message[i]!=0)
   {
      UARTTX[i] = _message[i];
      ++i;
   }
}

int main()
{
   // 128 bytes of incoming command space
   char incoming[32];
   unsigned int rcvcursor = 0;

   echoterm("NekoIchi\r\nrv32imf@60Mhz\r\nv0001\r\n");

   // Clear screen
   uint8_t bgcolor = 0x7C;
   uint32_t colorbits = (bgcolor<<24) | (bgcolor<<16) | (bgcolor<<8) | bgcolor;
   GPUFIFO[0] = GPUOPCODE(GPUSETREGISTER, 0, 1, GPU22BITIMM(colorbits));
   GPUFIFO[1] = GPUOPCODE(GPUSETREGISTER, 1, 1, GPU10BITIMM(colorbits));
   GPUFIFO[2] = GPUOPCODE(GPUCLEAR, 1, 0, 0);

   int x1 = 0;
   int y1 = 0;
   int x2 = 8;
   int y2 = 0;
   int x3 = 0;
   int y3 = 8;
   uint8_t ncolor = 0;
   int counter = 0;

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

            // Load the incoming binary
            if ((incoming[0]='b') && (incoming[1]=='i') && (incoming[2]=='n'))
               loadbinaryblob();
            if ((incoming[0]='r') && (incoming[1]=='u') && (incoming[2]=='n'))
               runbinary();
            // Rewind read cursor
            rcvcursor = 0;
         }

         // Echo the character back to the sender
         UARTTX[0] = checkchar;
         if (checkchar==13)
            UARTTX[1] = 10;

         // Wrap around if we're overflowing
         if (rcvcursor>31)
            rcvcursor = 0;
      }

      if (++counter > 131072)
      {
         // CLS
         GPUFIFO[0] = GPUOPCODE(GPUSETREGISTER, 0, 1, GPU22BITIMM(colorbits));
         GPUFIFO[1] = GPUOPCODE(GPUSETREGISTER, 1, 1, GPU10BITIMM(colorbits));
         GPUFIFO[2] = GPUOPCODE(GPUCLEAR, 1, 0, 0);

         // TRI
         uint32_t vertex10 = ((y2&0xFF)<<24) | ((x2&0xFF)<<16) | ((y1&0xFF)<<8) | (x1&0xFF);
         uint32_t vertexC2 = ((ncolor&0xFF)<<16) | ((y3&0xFF)<<8) | (x3&0xFF);
         GPUFIFO[0] = GPUOPCODE(GPUSETREGISTER, 0, 2, GPU22BITIMM(vertex10)); // {v1.y, v1.x, v0.y, v0.x}
         GPUFIFO[1] = GPUOPCODE(GPUSETREGISTER, 2, 2, GPU10BITIMM(vertex10));
         GPUFIFO[2] = GPUOPCODE(GPUSETREGISTER, 0, 3, GPU22BITIMM(vertexC2)); // {0, frontcolor, v2.y, v2.x}
         GPUFIFO[3] = GPUOPCODE(GPUSETREGISTER, 3, 3, GPU10BITIMM(vertexC2));
         GPUFIFO[4] = GPUOPCODE(GPURASTERIZE, 2, 3, 0);

         x1 += 8; x2 += 8; x3 += 8;
         if (x1 > 248)
         {
            x1 = 0; x2 = 8; x3 = 0;
            y1 = y1+8; y2 = y2+8; y3 = y3+8;
            if (y1 > 184)
            {
               y1 = 0; y2 = 0; y3 = 8;
            }
         }
         ++ncolor;
         counter = 0;         
      }

      // GPUFIFO[4] = GPUOPCODE(GPUVSYNC, 0, 0, 0);
   }

   return 0;
}
