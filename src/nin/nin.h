#ifndef NIN_H
#define NIN_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define RAM_SIZE    0x800

typedef struct {
    uint16_t    a;
    uint8_t     x;
    uint8_t     y;
    uint16_t    pc;
    uint8_t     s;
    uint8_t     p;
} NinCPU;

typedef struct {
    NinCPU      cpu;
    uint8_t*    ram;
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

#endif
