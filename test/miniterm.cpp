// MiniTerm
// Send to the SoC using:
// .\build\release\riscvtool.exe .\miniterm.elf -sendelf 0x00000210

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include "rvcrt0.h"

// 256x24 (3 rows, 32 characters on each row)
const uint8_t font[] __attribute__((aligned(4))) = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0xFF, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0xFF, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0xFF, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0xFF, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 
0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 
0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 
0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 
0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 
0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 
0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0xFF, 0x0, 0x0, 
0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 
};

volatile unsigned char* VRAM = (volatile unsigned char* )0x80000000;       // Video Output: VRAM starts at 0, continues for 0xC000 bytes (256x192 8 bit packed color pixels, RGB[3:3:2] format)
volatile unsigned char* UARTRX = (volatile unsigned char* )0x50000000;     // UART receive data (read)
volatile unsigned char* UARTTX = (volatile unsigned char* )0x40000000;     // UART send data (write)
volatile unsigned int* UARTRXStatus = (volatile unsigned int* )0x60000000; // UART input status (read)
volatile unsigned int targetjumpaddress = 0x00000000;
char chartable[32*24];

void print(int ox, int oy, int len, const char *message)
{
   int i=0;
   int charbyte, charrow, charcol, fontbyte;
   // Loop until we hit a null-terminator or end of string
   while (i<len && message[i] != 0)
   {
      charbyte = message[i]-32;
      charrow = (charbyte>>5)*8;
      charcol = (charbyte%32)*8;
      for (int y=0;y<8;++y)
      {
            int py = ((y+oy)<<8);
            for (int x=0;x<8;++x)
            {
               fontbyte = font[charcol+x + ((charrow+y)<<8)];
               if (fontbyte)
                  VRAM[i*8+x+ox+py] = 0xFF;
            }
      }
      ++i;
   }
}

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

// Clear the screen
void cls(const unsigned char color)
{
   for(int y=0;y<192;++y)
   {
      int py = y<<8;
      for(int x=0;x<256;++x)
         VRAM[x+py] = color;
   }
}

void clearchars()
{
   for (int cy=0;cy<24;++cy)
      for (int cx=0;cx<32;++cx)
         chartable[cx+cy*32] = ' ';
}

void scroll()
{
   for (int cy=0;cy<23;++cy)
      for (int cx=0;cx<32;++cx)
         chartable[cx+cy*32] = chartable[cx+(cy+1)*32];

   // Clear the last row
   for (int cx=0;cx<32;++cx)
      chartable[cx+23*32] = ' ';
}

int main()
{
   // 32 bytes of incoming command space
   char incoming[32];

   unsigned int rcvcursor = 0;
   unsigned int cmdcounter = 23;
   unsigned int oldcount = 0;

   // Startup message
   clearchars();
   cls(0x0F);
   print(0, 184, 30, "MiniTerm (c)2021 Engin Cilasun");

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

         if (checkchar == 8 && rcvcursor!=1) // Backspace? (make sure your terminal uses ctrl+h for backspace)
         {
            --rcvcursor;
            incoming[rcvcursor-1] = 0;
            // Copy the string to the chartable
            for (int i=0;i<rcvcursor;++i)
               chartable[i+cmdcounter*32] = incoming[i];
            cls(0x0F);
            // Show the char table
            for (int cy=0;cy<24;++cy)
               for (int cx=0;cx<32;++cx)
                  print(8*cx, 8*cy, 1, &chartable[cx+cy*32]);
            --rcvcursor;
         }
         else if (checkchar == 13) // Enter?
         {
            // Terminate the string
            incoming[rcvcursor-1] = 0;

            // Copy the string to the chartable
            for (int i=0;i<rcvcursor;++i)
               chartable[i+cmdcounter*32] = incoming[i];

            // Step down or scroll
            cls(0x0F);
            scroll();

            // Show the char table
            for (int cy=0;cy<24;++cy)
               for (int cx=0;cx<32;++cx)
                  print(8*cx, 8*cy, 1, &chartable[cx+cy*32]);

            // Clear the command line area at bottom 8 pixels of the screen
            for(int y=184;y<192;++y)
            {
               int py = y<<8;
               for(int x=0;x<256;++x)
                  VRAM[x+py] = 0x0F;
            }

            // Clear the whole screen
            //if (!strcmp(incoming, "cls"))
            if (incoming[0]='c' && incoming[1]=='l' && incoming[2]=='s')
            {
               clearchars();
               cls(0x0F);
            }

            // Load incoming binary from UART
            //if (!strcmp(incoming, "load"))
            if (incoming[0]='l' && incoming[1]=='o' && incoming[2]=='a' && incoming[3]=='d')
               targetjumpaddress = load();

            if (incoming[0]='r' && incoming[1]=='u' && incoming[2]=='n')
            {
               targetjumpaddress = load();
               ((void (*)(void)) targetjumpaddress)();
            }

            // Rewind read cursor
            rcvcursor=0;
         }
         else
         {
            // Print out this many characters
            print(0, 184, rcvcursor, incoming);
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
