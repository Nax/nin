#ifndef LIBNIN_H
#define LIBNIN_H

#include <nin/nin.h>

#include <stdlib.h>

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

#define NMI_OCCURED 0x01
#define NMI_OUTPUT  0x02

#define BITMAP_X    256
#define BITMAP_Y    240

typedef struct {
    uint8_t     mode;
    uint8_t     op;
    uint8_t     len;
    uint8_t     data;
    uint16_t    addr;
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
    uint16_t    t;
    uint16_t    v;
    uint8_t     x;
    uint16_t    scanline;
    uint16_t    cycle;
    uint8_t     latchName;
    uint8_t     latchAttr;
    uint8_t     latchTileLo;
    uint8_t     latchTileHi;
    uint16_t    shiftPatternLo;
    uint16_t    shiftPatternHi;
    uint16_t    shiftPaletteHi;
    uint16_t    shiftPaletteLo;
    uint8_t     oam2[64];
    uint8_t     latchSpriteBitmapLo[8];
    uint8_t     latchSpriteBitmapHi[8];
    uint8_t     latchSpriteBitmapAttr[8];
    uint8_t     latchSpriteBitmapX[8];
    unsigned    zeroHit:1;
    unsigned    zeroHitNext:1;
    unsigned    maskEnableBackground:1;
    unsigned    maskEnableSprites:1;
} NinRuntimePPU;

typedef struct {
    NinRuntimePPU   rt;
    uint8_t         readBuf;
    uint8_t         latch;
    uint8_t         nmi;
    uint8_t         controller;
    uint8_t         scrollX;
    uint8_t         scrollY;
    unsigned        w:1;
    unsigned        zeroHitFlag:1;
} NinPPU;

typedef struct {
    uint16_t        triangleTimer;
    uint16_t        triangleClock;
    uint8_t         triangleSeq;
    uint16_t        pulseTimer[2];
    uint16_t        pulseClock[2];
    uint8_t         pulseSeq[2];
    unsigned        half:1;
} NinAPU;

struct NinState_ {
    uint8_t             controller;
    uint8_t             controllerLatch;
    uint32_t*           bitmap;
    int16_t*            audioSamples;
    uint16_t            audioSamplesCount;
    uint32_t            audioCycles;
    NINAUDIOCALLBACK    audioCallback;
    void*               audioCallbackArg;
    NinCPU              cpu;
    NinPPU              ppu;
    NinAPU              apu;
    NinTraceCache*      traceCache;
    uint8_t*            ram;
    uint8_t*            vram;
    uint8_t*            palettes;
    uint8_t*            oam;
    uint8_t*            prgRom;
    uint8_t*            chrRom;
    uint32_t            prgRomSize;
    uint32_t            chrRomSize;
    unsigned            nmi:1;
};

uint8_t     ninMemoryRead8(NinState* state, uint16_t addr);
uint16_t    ninMemoryRead16(NinState* state, uint16_t addr);
void        ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);
void        ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value);

uint8_t     ninVMemoryRead8(NinState* state, uint16_t addr);
void        ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);

void        ninRunCycles(NinState* state, size_t cycles);
void        ninSetFlagNMI(NinState* state, uint8_t flag);
void        ninUnsetFlagNMI(NinState* state, uint8_t flag);

uint8_t     ninPpuRegRead(NinState* state, uint16_t reg);
void        ninPpuRegWrite(NinState* state, uint16_t reg, uint8_t value);

uint8_t     ninApuRegRead(NinState* state, uint16_t reg);
void        ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value);

NinTrace*   ninGetTrace(NinState* state, uint16_t addr);
void        ninPpuRenderFrame(NinState* state);
int         ninPpuRunCycles(NinState* state, uint16_t cycles);

void        ninRunFrameCPU(NinState* state);
void        ninRunCyclesAPU(NinState* state, size_t cycles);

#endif
