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

#ifndef LIBNIN_MAPPER_H
#define LIBNIN_MAPPER_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

#define NIN_MIRROR_A 0
#define NIN_MIRROR_B 1
#define NIN_MIRROR_H 2
#define NIN_MIRROR_V 3

namespace libnin
{

class Memory;
class Cart;
class IRQ;
class Mapper : private NonCopyable
{
public:
    Mapper(Memory& memory, Cart& cart, IRQ& irq);
    ~Mapper();

    const std::uint8_t* bank(int slot) const { return _banks[slot]; }
    std::uint8_t* chr(int i) { return _chr[i]; }
    std::uint8_t* nametable(int i) { return _nametables[i]; }

    bool configure(std::uint16_t mapperMajor, std::uint8_t mapperMinor);
    void mirror(int mirrorMode);

    void            tick(void) { (this->*_tickHandler)(); }
    std::uint8_t    read(std::uint16_t addr);
    void            write(std::uint16_t addr, std::uint8_t value);
    void            handleWrite(std::uint16_t addr, std::uint8_t value) { (this->*_writeHandler)(addr, value); }

    void videoRead(std::uint16_t addr) { (this->*_videoReadHandler)(addr); }

    void bankPrg8k(std::uint8_t slot, int domain, std::int16_t bank);
    void bankPrg16k(std::uint8_t slot, int domain, std::int16_t bank);
    void bankPrg32k(std::uint8_t slot, int domain, std::int16_t bank);

    void bankChr1k(std::uint8_t slot, std::int16_t bank);
    void bankChr2k(std::uint8_t slot, std::int16_t bank);
    void bankChr4k(std::uint8_t slot, std::int16_t bank);
    void bankChr8k(std::int16_t bank);

private:
    using TickHandler = void(Mapper::*)(void);
    using ReadHandler = std::uint8_t(Mapper::*)(std::uint16_t);
    using WriteHandler = void(Mapper::*)(std::uint16_t, std::uint8_t);
    using VideoReadHandler = void(Mapper::*)(std::uint16_t);

    struct MMC1
    {
        std::uint8_t shift;
        std::uint8_t count;
        std::uint8_t mirroring:2;
        std::uint8_t prgBankMode:2;
        std::uint8_t chrBankMode:1;
        std::uint8_t chrBank0:5;
        std::uint8_t chrBank1:5;
        std::uint8_t prgBank:4;
    };

    struct MMC2
    {
        std::uint8_t bankLatch0Lo;
        std::uint8_t bankLatch0Hi;
        std::uint8_t bankLatch1Lo;
        std::uint8_t bankLatch1Hi;
        std::uint8_t latch0:1;
        std::uint8_t latch1:1;
    };

    struct MMC3
    {
        std::uint8_t bankSelect:3;
        std::uint8_t bank[8];
        std::uint8_t bankModePrgRom:1;
        std::uint8_t bankModeChrRom:1;

        std::uint8_t    irqScanlineEnabled:1;
        std::uint8_t    irqScanlineReload:1;
        std::uint8_t    irqScanlineCounter;
        std::uint8_t    irqScanlineReloadValue;
        std::uint16_t   irqScanlineFilterShifter;
        std::uint16_t   oldVmemAddr;
    };

    struct MMC5
    {
        std::uint8_t    bankModePrg:2;
        std::uint8_t    bankModeChr:2;
        std::uint8_t    bankSelectPrg[5];
        std::uint8_t    bankSelectChr[12];
        std::uint8_t    mul[2];

        std::uint16_t   ppuAddr;
        std::uint8_t    ppuAddrCount;
        std::uint8_t    ppuIdleCount;
        std::uint8_t    scanline;
        std::uint8_t    scanlineTarget;

        bool ppuReading:1;
        bool ppuSpriteFlag:1;
        bool ppuRenderFlag:1;
        bool mode8x16:1;
        bool inFrame:1;
        bool scanlineEnabled:1;
        bool scanlinePending:1;
    };

    void tick_NROM(void);
    void tick_MMC5(void);

    std::uint8_t read_NROM(std::uint16_t addr);
    std::uint8_t read_MMC5(std::uint16_t addr);

    void write_NROM(std::uint16_t addr, std::uint8_t value);
    void write_UXROM(std::uint16_t addr, std::uint8_t value);
    void write_UXROM180(std::uint16_t addr, std::uint8_t value);
    void write_CNROM(std::uint16_t addr, std::uint8_t value);
    void write_GXROM(std::uint16_t addr, std::uint8_t value);
    void write_AXROM(std::uint16_t addr, std::uint8_t value);
    void write_ColorDreams(std::uint16_t addr, std::uint8_t value);
    void write_MMC1(std::uint16_t addr, std::uint8_t value);
    void write_MMC2(std::uint16_t addr, std::uint8_t value);
    void write_MMC3(std::uint16_t addr, std::uint8_t value);
    void write_MMC4(std::uint16_t addr, std::uint8_t value);
    void write_MMC5(std::uint16_t addr, std::uint8_t value);

    void videoRead_NULL(std::uint16_t);
    void videoRead_MMC2(std::uint16_t);
    void videoRead_MMC3(std::uint16_t);
    void videoRead_MMC5(std::uint16_t);

    void mmc1RegWrite(std::uint16_t addr, std::uint8_t value);
    void mmc1BankPrg();
    void mmc1BankChr();
    void mmc1Mirror();

    void mmc2Common(std::uint16_t addr, std::uint8_t value);
    void mmc2Apply();

    void mmc3Apply();

    void mmc5ApplyPRG();
    void mmc5ApplyCHR();

    Memory& _memory;
    Cart&   _cart;
    IRQ&    _irq;

    TickHandler         _tickHandler;
    ReadHandler         _readHandler;
    WriteHandler        _writeHandler;
    VideoReadHandler    _videoReadHandler;

    std::uint8_t*   _banks[6];
    bool            _banksWriteFlag[6];
    std::uint8_t*   _chr[8];
    std::uint8_t*   _nametables[4];

    union
    {
        MMC1 _mmc1;
        MMC2 _mmc2;
        MMC3 _mmc3;
        MMC5 _mmc5;
    };
};

}

#endif
