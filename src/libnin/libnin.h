/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBNIN_H
#define LIBNIN_H

#include <nin/nin.h>
#include <stdlib.h>

#define UNUSED(x)   ((void)x)

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

typedef void    (*NinPrgWriteHandler)(struct NinState_* state, uint16_t addr, uint8_t value);
typedef void    (*NinPpuMonitorHandler)(struct NinState_* state, uint16_t addr);
typedef uint8_t (*NinMemoryReadHandler)(struct NinState_* state, uint16_t addr);
typedef void    (*NinMemoryWriteHandler)(struct NinState_* state, uint16_t addr, uint8_t value);

#define MIRRORING_HORIZONTAL    0
#define MIRRORING_VERTICAL      1
#define MIRRORING_QUAD          2

#define AUDIO_BUFFER_SIZE_SOURCE_NTSC     221
#define AUDIO_BUFFER_SIZE_TARGET          1024

#define CLOCK_RATE_NTSC     1789773

template <typename T>
inline static T* zalloc(size_t count)
{
    if (count)
        return new T[count]();
    return nullptr;
}

typedef struct {
    uint8_t reserved[8];
} NinRomHeaderLegacy;

typedef struct {
    uint8_t     mapperEx:4;
    uint8_t     submapper:4;
    uint8_t     prgRomSizeHi:4;
    uint8_t     chrRomSizeHi:4;
    uint8_t     prgRamSizeShift:4;
    uint8_t     prgNvramSizeShift:4;
    uint8_t     chrRamSizeShift:4;
    uint8_t     chrNvramSizeShift:4;
    uint8_t     region:2;
    uint8_t     reserved0:6;
    uint8_t     reserved[3];

} NinRomHeaderNes2;

typedef struct {
    char        magic[4];
    uint8_t     prgRomSize;
    uint8_t     chrRomSize;
    uint8_t     mirroring:1;
    uint8_t     battery:1;
    uint8_t     trainer:1;
    uint8_t     quadScreen:1;
    uint8_t     mapperLo:4;
    uint8_t     vs:1;
    uint8_t     playChoice:1;
    uint8_t     magicNes2:2;
    uint8_t     mapperHi:4;
    union {
        NinRomHeaderLegacy  ines;
        NinRomHeaderNes2    nes2;
    };
} NinRomHeader;

typedef struct {
    uint16_t    pc;
    uint8_t     regs[4];
    uint8_t     p;
    uint8_t     p2;
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

typedef struct {
    uint8_t bankLatch0Lo;
    uint8_t bankLatch0Hi;
    uint8_t bankLatch1Lo;
    uint8_t bankLatch1Hi;
    uint8_t latch0:1;
    uint8_t latch1:1;
} NinMapperRegsMMC2;

typedef struct {
    uint8_t bankSelect:3;
    uint8_t bank[8];
    uint8_t bankModePrgRom:1;
    uint8_t bankModeChrRom:1;
} NinMapperRegsMMC3;

typedef union {
    NinMapperRegsMMC1 mmc1;
    NinMapperRegsMMC2 mmc2;
    NinMapperRegsMMC3 mmc3;
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
    uint8_t     inhibitNmi:1;
} NinRuntimePPU;

typedef struct {
    NinRuntimePPU   rt;
    uint8_t         oamAddr;
    uint8_t         readBuf;
    uint8_t         latch;
    uint8_t         nmi;
    uint8_t         controller;
    uint8_t         scrollX;
    uint8_t         scrollY;
    uint8_t         w:1;
    uint8_t         zeroHitFlag:1;
    uint8_t         race0:1;
    uint8_t         race1:1;
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
    uint8_t             mode:1;
    uint8_t             irqInhibit:1;
    uint8_t             resetClock;
} NinAPU;

typedef struct {
    float               samplesDst[AUDIO_BUFFER_SIZE_TARGET];
    double              loPassX[6];
    double              loPassY[6];
    double              hiPassX[4];
    double              hiPassY[4];
    uint16_t            samplesCursorDst;
    uint32_t            dividerNum;
    uint32_t            dividerDen;
    uint32_t            dividerClock;
    NINAUDIOCALLBACK    callback;
    void*               callbackArg;
} NinAudio;

typedef struct
{
    uint32_t            clockRate;
    uint32_t            frameCycles;
    uint32_t            frameDelay;
    uint8_t             cycleExtraCounter;
    uint8_t             cycleExtraIncrement;
    uint8_t             firstVisibleScanline;
    uint8_t             vblank;
    const uint16_t*     apuFrameCycles;
    const uint16_t*     apuNoisePeriod;
    const uint16_t*     apuDmcPeriod;
} NinRegionData;

typedef struct {
    uint8_t     extPort;
    uint8_t     motor:1;
    uint8_t     noScan:1;
    uint8_t     noWrite:1;
    uint8_t     inData:1;
    uint8_t     irqEnabledTransfer:1;
    uint8_t     irqEnabledTimer:1;
    uint8_t     irqReloadFlag:1;
    uint16_t    irqReloadValue;
    uint16_t    irqTimer;
    uint8_t     transfered:1;
    uint8_t     latchRead;
    uint16_t    headClock;
    uint32_t    headPos;
    uint16_t    delay;
    uint8_t     scanning:1;
    uint8_t     skippedGap:1;
    uint8_t     endOfDisk:1;
} NinFds;

struct NinState_ {
    FILE*               saveFile;
    NinRegion           region;
    NinRegionData       regionData;
    NinAudio            audio;
    uint16_t            mapper;
    uint8_t             battery;
    uint8_t             mirroring;
    uint8_t             controller;
    uint8_t             controllerLatch;
    uint32_t*           backBuffer;
    uint32_t*           frontBuffer;
    float               audioSampleLoLast;
    float               audioSampleHiLast[2];
    float               audioSampleHiLastRaw[2];
    uint16_t            audioSamplesCount;
    uint32_t            audioCycles;
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

    NinMapperRegs           mapperRegs;
    NinPrgWriteHandler      prgWriteHandler;
    NinPpuMonitorHandler    ppuMonitorHandler;
    NinMemoryReadHandler    readHandler;
    NinMemoryWriteHandler   writeHandler;

    uint8_t             diskSides;
    uint32_t            diskDataSize;
    uint8_t*            diskData;
    uint8_t             prgBankCount;
    uint8_t*            prgRom;
    uint32_t            prgRomSize;
    uint8_t*            prgRomBank[4];
    uint8_t*            prgRam;
    uint16_t            prgRamSize;
    uint16_t            chrBankCount;
    uint8_t*            chrRom;
    uint32_t            chrRomSize;
    uint8_t*            chrRam;
    uint32_t            chrRamSize;
    uint8_t*            chrBank[8];
    uint32_t            trainerSize;
    uint8_t*            nametables[4];
    uint8_t             nmi:1;
    uint8_t             nmi2:1;
    uint8_t             irq:1;
    uint8_t             irqFlags;
    uint64_t            cyc;
    uint8_t             frame:1;
    uint8_t             frameOdd:1;
    uint8_t             irqScanlineEnabled:1;
    uint8_t             irqScanlineReload:1;
    uint8_t             irqScanlineCounter;
    uint8_t             irqScanlineReloadValue;
    uint16_t            irqScanlineFilterShifter;
    uint16_t            oldVmemAddr;
    NinSystem           system;
    NinFds              fds;
};

#define IRQ_APU_FRAME       0x01
#define IRQ_APU_DMC         0x02
#define IRQ_SCANLINE        0x04
#define IRQ_FDS_TRANSFER    0x08
#define IRQ_FDS_TIMER       0x10

void        ninApplyMapper(NinState* state, uint8_t mapperNum);

uint8_t     ninMemoryRead8(NinState* state, uint16_t addr);
uint16_t    ninMemoryRead16(NinState* state, uint16_t addr);
void        ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);
void        ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value);

uint8_t     ninMemoryReadNES(NinState* state, uint16_t addr);
uint8_t     ninMemoryReadFDS(NinState* state, uint16_t addr);
void        ninMemoryWriteNES(NinState* state, uint16_t addr, uint8_t value);
void        ninMemoryWriteFDS(NinState* state, uint16_t addr, uint8_t value);

uint8_t     ninVMemoryRead8(NinState* state, uint16_t addr);
void        ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);

void        ninSetFlagNMI(NinState* state, uint8_t flag);
void        ninUnsetFlagNMI(NinState* state, uint8_t flag);

uint8_t     ninPpuRegRead(NinState* state, uint16_t reg);
void        ninPpuRegWrite(NinState* state, uint16_t reg, uint8_t value);

uint8_t     ninApuRegRead(NinState* state, uint16_t reg);
void        ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value);

int         ninPpuRunCycles(NinState* state, uint16_t cycles);
void        ninRunCyclesAPU(NinState* state, size_t cycles);

void        ninSetIRQ(NinState* state, uint8_t flag);
void        ninClearIRQ(NinState* state, uint8_t flag);

/* Audio */
void    ninAudioSetFrequencySource(NinState* state, uint32_t freq);
void    ninAudioPushSample(NinState* state, float sample);

/* Region */
void    ninRegionApply(NinState* state);

/* Mapper handlers */
void    ninPrgWriteHandlerNull(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC1(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC2(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC3(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC4(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerUXROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerUXROM_180(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerCNROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerAXROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerGXROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerColorDreams(NinState* state, uint16_t addr, uint8_t value);

void    ninPrgWriteHandlerCommonMMC2_MMC4(NinState* state, uint16_t addr, uint8_t value);

void    ninPpuMonitorHandlerNull(NinState* state, uint16_t addr);
void    ninPpuMonitorHandlerMMC2(NinState* state, uint16_t addr);
void    ninPpuMonitorHandlerMMC3(NinState* state, uint16_t addr);

/* Bank */
void    ninBankSwitchPrgRom8k(NinState* state, uint8_t slot, uint8_t bank);
void    ninBankSwitchPrgRom16k(NinState* state, uint8_t slot, uint8_t bank);
void    ninBankSwitchPrgRom32k(NinState* state, uint8_t bank);

void    ninBankSwitchChrRom1k(NinState* state, uint8_t slot, uint8_t bank);
void    ninBankSwitchChrRom4k(NinState* state, uint8_t slot, uint8_t bank);
void    ninBankSwitchChrRom8k(NinState* state, uint8_t bank);

/* mirror.c */
void    ninMirrorA(NinState* state);
void    ninMirrorB(NinState* state);
void    ninMirrorH(NinState* state);
void    ninMirrorV(NinState* state);

/* fds.c */
uint8_t     ninFdsRegRead(NinState* state, uint16_t addr);
void        ninFdsRegWrite(NinState* state, uint16_t addr, uint8_t value);
void        ninFdsCycle(NinState* state);

#endif
