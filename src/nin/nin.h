#ifndef NIN_H
#define NIN_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define RAM_SIZE    0x800
#define VRAM_SIZE   0x800

#define PFLAG_C     0x01
#define PFLAG_Z     0x02
#define PFLAG_I     0x04
#define PFLAG_D     0x08
#define PFLAG_B     0x10
#define PFLAG_1     0x20
#define PFLAG_V     0x40
#define PFLAG_N     0x80

#define PFLAG_MASK  (~(PFLAG_B | PFLAG_1))

typedef struct {
    uint16_t    pc;
    uint8_t     a;
    uint8_t     x;
    uint8_t     y;
    uint8_t     s;
    uint8_t     p;
} NinCPU;

typedef struct {
    uint16_t    addr;
    uint8_t     latch;
    unsigned    addrHalfFlag:1;
    unsigned    dataHalfFlag:1;
} NinPPU;

typedef struct {
    NinCPU      cpu;
    NinPPU      ppu;
    uint8_t*    ram;
    uint8_t*    vram;
    uint8_t*    prgRom;
    uint8_t*    chrRom;
    uint32_t    prgRomSize;
    uint32_t    chrRomSize;
} NinState;

NinState*   ninCreateState(FILE* rom);
void        ninDestroyState(NinState* state);

uint8_t     ninMemoryRead8(NinState* state, uint16_t addr);
uint16_t    ninMemoryRead16(NinState* state, uint16_t addr);
void        ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);
void        ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value);

uint8_t     ninVMemoryRead8(NinState* state, uint16_t addr);
void        ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);

void        ninRunCycles(NinState* state, size_t cycles);

uint8_t     ninPpuRegRead(NinState* state, uint16_t reg);
void        ninPpuRegWrite(NinState* state, uint16_t reg, uint8_t value);

#endif
