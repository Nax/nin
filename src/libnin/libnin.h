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

#define REG_A   0x00
#define REG_X   0x01
#define REG_Y   0x02
#define REG_S   0x03

#define NMI_OCCURED 0x01
#define NMI_OUTPUT  0x02

#define BITMAP_X    256
#define BITMAP_Y    240

#define DEBUG_LEVEL 0

typedef int (*NinPrgWriteHandler)(struct NinState_* state, uint16_t addr, uint8_t value);

typedef struct {
    uint16_t    pc;
    uint8_t     regs[4];
    uint8_t     p;
} NinCPU;

typedef struct {
    uint8_t shift;
    uint8_t count;
    uint8_t mirroring:2;
    uint8_t prgBankMode:2;
    uint8_t chrBankMode:1;
    uint8_t chrBank0:5;
    uint8_t chrBank1:5;
    uint8_t prgBank:4;
} NinMapperRegsMMC1;

typedef union {
    NinMapperRegsMMC1 mmc1;
} NinMapperRegs;

typedef union {
    uint8_t raw[4];
    struct {
        uint8_t y;
        uint8_t tile;
        uint8_t palette:2;
        uint8_t reserved:3;
        uint8_t back:1;
        uint8_t xFlip:1;
        uint8_t yFlip:1;
        uint8_t x;
    };
} NinSprite;

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
    NinSprite   oam2[8];
    uint8_t     oam2Index;
    uint8_t     latchSpriteBitmapLo[8];
    uint8_t     latchSpriteBitmapHi[8];
    uint8_t     latchSpriteBitmapAttr[8];
    uint8_t     latchSpriteBitmapX[8];
    unsigned    zeroHit:1;
    unsigned    zeroHitNext:1;
    unsigned    maskEnableBackground:1;
    unsigned    maskEnableBackgroundLeft:1;
    unsigned    maskEnableSprites:1;
    unsigned    maskEnableSpritesLeft:1;
    unsigned    largeSprites:1;
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

typedef union {
    uint8_t     raw;
    struct {
        unsigned shift:3;
        unsigned negate:1;
        unsigned divider:3;
        unsigned enabled:1;
    };
} NinAudioSweep;

typedef struct {
    uint16_t    timerPeriod;
    uint16_t    timerValue;
    uint8_t     seqIndex;
    uint8_t     linear;
    uint8_t     linearReload:7;
    uint8_t     linearReloadFlag:1;
    uint8_t     length;
    uint8_t     control:1;
    uint8_t     enabled:1;
} NinChannelTriangle;

typedef struct {
    uint8_t volume:4;
    uint8_t decay:4;
    uint8_t divider:4;
    uint8_t halt:1;
    uint8_t constant:1;
    uint8_t start:1;
} NinEnvelope;

typedef struct {
    uint16_t    timerPeriod;
    uint16_t    timerValue;
    uint8_t     seqIndex;
    uint8_t     length;
    uint8_t     duty;
    NinEnvelope envelope;
    uint8_t     sweepEnable:1;
    uint8_t     sweepPeriod:4;
    uint8_t     sweepValue:4;
    uint8_t     sweepNegate:1;
    uint8_t     sweepShift:3;
    uint16_t    sweepTarget;
    uint8_t     sweepReload:1;
    uint8_t     enabled:1;
} NinChannelPulse;

typedef struct {
    uint16_t        timerPeriod;
    uint16_t        timerValue;
    uint16_t        feedback;
    uint8_t         length;
    uint8_t         mode:1;
    uint8_t         enabled:1;
    NinEnvelope     envelope;
} NinChannelNoise;

typedef struct {
    uint16_t        timerPeriod;
    uint16_t        timerValue;
    uint16_t        address;
    uint16_t        length;
    uint8_t         output;
    uint8_t         irqEnable:1;
    uint8_t         irq:1;
    uint8_t         loop:1;
    uint8_t         enabled:1;
    uint8_t         sampleBuffer;
    uint8_t         bitCount:4;
    uint8_t         silence:1;
} NinChannelDMC;

typedef struct {
    uint16_t            frameCounter;
    NinChannelTriangle  triangle;
    NinChannelPulse     pulse[2];
    NinChannelNoise     noise;
    NinChannelDMC       dmc;
    uint8_t             half:1;
    uint8_t             mode:1;
} NinAPU;

struct NinState_ {
    FILE*               saveFile;
    uint8_t             controller;
    uint8_t             controllerLatch;
    uint32_t*           backBuffer;
    uint32_t*           frontBuffer;
    float*              audioSamples;
    float               audioSampleLoLast;
    float               audioSampleHiLast[2];
    float               audioSampleHiLastRaw[2];
    uint16_t            audioSamplesCount;
    int16_t*            audioSamplesFiltered;
    uint32_t            audioCycles;
    NINAUDIOCALLBACK    audioCallback;
    void*               audioCallbackArg;
    NinCPU              cpu;
    NinPPU              ppu;
    NinAPU              apu;
    uint8_t*            ram;
    uint8_t*            vram;
    uint8_t*            palettes;
    union {
        uint8_t*        oam;
        NinSprite*      oamSprites;
    };
    NinMapperRegs       mapper;
    uint8_t             prgBankCount;
    uint8_t*            prgRom;
    uint32_t            prgRomSize;
    uint8_t*            prgRomBank[2];
    NinPrgWriteHandler  prgWriteHandler;
    uint8_t*            prgRam;
    uint16_t            prgRamSize;
    uint8_t             chrBankCount;
    uint8_t*            chrRom;
    uint32_t            chrRomSize;
    uint8_t*            chrRam;
    uint32_t            chrRamSize;
    uint8_t*            chrBank[2];
    uint8_t*            nametables[4];
    unsigned            nmi:1;
    uint64_t            cyc;
    uint8_t             frame:1;
};

void        ninApplyMapper(NinState* state, uint8_t mapperNum);

NIN_API uint8_t     ninMemoryRead8(NinState* state, uint16_t addr);
NIN_API uint16_t    ninMemoryRead16(NinState* state, uint16_t addr);
NIN_API int         ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);
NIN_API int         ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value);

NIN_API uint8_t     ninVMemoryRead8(NinState* state, uint16_t addr);
NIN_API void        ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);

void        ninSetFlagNMI(NinState* state, uint8_t flag);
void        ninUnsetFlagNMI(NinState* state, uint8_t flag);

NIN_API uint8_t     ninPpuRegRead(NinState* state, uint16_t reg);
NIN_API void        ninPpuRegWrite(NinState* state, uint16_t reg, uint8_t value);

uint8_t     ninApuRegRead(NinState* state, uint16_t reg);
void        ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value);

NIN_API int         ninPpuRunCycles(NinState* state, uint16_t cycles);
NIN_API void        ninRunFrameCPU(NinState* state);
NIN_API void        ninRunCyclesAPU(NinState* state, size_t cycles);

/* Mapper handlers */
NIN_API void    ninPrgWriteHandlerNull(NinState* state, uint16_t addr, uint8_t value);
NIN_API void    ninPrgWriteHandlerMMC1(NinState* state, uint16_t addr, uint8_t value);
NIN_API void    ninPrgWriteHandlerUXROM(NinState* state, uint16_t addr, uint8_t value);
NIN_API void    ninPrgWriteHandlerCNROM(NinState* state, uint16_t addr, uint8_t value);

#endif
