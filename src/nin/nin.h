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

#define TRACE_MAX_UOPS      64
#define TRACE_CACHE_SIZE    512
#define TRACE_NONE          0xFFFF

#define REG_A   0x00
#define REG_X   0x01
#define REG_Y   0x02
#define REG_S   0x03

typedef struct {
    uint8_t     op;
    uint8_t     len;
    uint16_t    data;
} NinUop;

typedef struct {
    uint16_t    pc;
    uint16_t    length;
    NinUop      uops[TRACE_MAX_UOPS];
} NinTrace;

typedef struct {
    uint16_t    index[0x10000];
    NinTrace    traces[TRACE_CACHE_SIZE];
    uint16_t    cursor;
} NinTraceCache;

typedef struct {
    uint16_t    pc;
    uint8_t     regs[4];
    uint8_t     p;
} NinCPU;

typedef struct {
    uint16_t    addr;
    uint8_t     latch;
    unsigned    addrHalfFlag:1;
    unsigned    dataHalfFlag:1;
} NinPPU;

typedef struct {
    NinCPU          cpu;
    NinPPU          ppu;
    NinTraceCache*  traceCache;
    uint8_t*        ram;
    uint8_t*        vram;
    uint8_t*        prgRom;
    uint8_t*        chrRom;
    uint32_t        prgRomSize;
    uint32_t        chrRomSize;
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

NinTrace*   ninGetTrace(NinState* state, uint16_t addr);

#endif
