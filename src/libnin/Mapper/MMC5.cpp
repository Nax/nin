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

#include <libnin/Cart.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/Util.h>

namespace libnin
{

static void applyPrg(Mapper& mapper, MapperMMC5& mmc5)
{
    mapper.bankPrg8k(1, CART_PRG_RAM, mmc5.bankSelectPrg[0]);
    switch (mmc5.bankModePrg)
    {
    case 0:
        mapper.bankPrg8k(2, CART_PRG_ROM, (mmc5.bankSelectPrg[4] & 0x7c) | 0x00);
        mapper.bankPrg8k(3, CART_PRG_ROM, (mmc5.bankSelectPrg[4] & 0x7c) | 0x01);
        mapper.bankPrg8k(4, CART_PRG_ROM, (mmc5.bankSelectPrg[4] & 0x7c) | 0x02);
        mapper.bankPrg8k(5, CART_PRG_ROM, (mmc5.bankSelectPrg[4] & 0x7c) | 0x03);
        break;
    case 1:
        mapper.bankPrg8k(2, (mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (mmc5.bankSelectPrg[2] & 0x7e) | 0x00);
        mapper.bankPrg8k(3, (mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (mmc5.bankSelectPrg[2] & 0x7e) | 0x01);
        mapper.bankPrg8k(4, CART_PRG_ROM, (mmc5.bankSelectPrg[4] & 0x7e) | 0x00);
        mapper.bankPrg8k(5, CART_PRG_ROM, (mmc5.bankSelectPrg[4] & 0x7e) | 0x01);
        break;
    case 2:
        mapper.bankPrg8k(2, (mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (mmc5.bankSelectPrg[2] & 0x7e) | 0x00);
        mapper.bankPrg8k(3, (mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (mmc5.bankSelectPrg[2] & 0x7e) | 0x01);
        mapper.bankPrg8k(4, (mmc5.bankSelectPrg[3] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, mmc5.bankSelectPrg[3] & 0x7f);
        mapper.bankPrg8k(5, CART_PRG_ROM, mmc5.bankSelectPrg[4] & 0x7f);
        break;
    case 3:
        mapper.bankPrg8k(2, (mmc5.bankSelectPrg[1] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, mmc5.bankSelectPrg[1] & 0x7f);
        mapper.bankPrg8k(3, (mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, mmc5.bankSelectPrg[2] & 0x7f);
        mapper.bankPrg8k(4, (mmc5.bankSelectPrg[3] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, mmc5.bankSelectPrg[3] & 0x7f);
        mapper.bankPrg8k(5, CART_PRG_ROM, mmc5.bankSelectPrg[4] & 0x7f);
        break;
    }
}

static void applyChr(Mapper& mapper, MapperMMC5& mmc5)
{
    switch (mmc5.bankModeChr)
    {
    case 0:
        mapper.bankChr1k(0, mmc5.bankSelectChr[7] + 0);
        mapper.bankChr1k(1, mmc5.bankSelectChr[7] + 1);
        mapper.bankChr1k(2, mmc5.bankSelectChr[7] + 2);
        mapper.bankChr1k(3, mmc5.bankSelectChr[7] + 3);
        mapper.bankChr1k(4, mmc5.bankSelectChr[7] + 4);
        mapper.bankChr1k(5, mmc5.bankSelectChr[7] + 5);
        mapper.bankChr1k(6, mmc5.bankSelectChr[7] + 6);
        mapper.bankChr1k(7, mmc5.bankSelectChr[7] + 7);
        break;
    case 1:
        mapper.bankChr1k(0, mmc5.bankSelectChr[3] + 0);
        mapper.bankChr1k(1, mmc5.bankSelectChr[3] + 1);
        mapper.bankChr1k(2, mmc5.bankSelectChr[3] + 2);
        mapper.bankChr1k(3, mmc5.bankSelectChr[3] + 3);
        mapper.bankChr1k(4, mmc5.bankSelectChr[7] + 0);
        mapper.bankChr1k(5, mmc5.bankSelectChr[7] + 1);
        mapper.bankChr1k(6, mmc5.bankSelectChr[7] + 2);
        mapper.bankChr1k(7, mmc5.bankSelectChr[7] + 3);
        break;
    case 2:
        mapper.bankChr1k(0, mmc5.bankSelectChr[1] + 0);
        mapper.bankChr1k(1, mmc5.bankSelectChr[1] + 1);
        mapper.bankChr1k(2, mmc5.bankSelectChr[3] + 0);
        mapper.bankChr1k(3, mmc5.bankSelectChr[3] + 1);
        mapper.bankChr1k(4, mmc5.bankSelectChr[5] + 0);
        mapper.bankChr1k(5, mmc5.bankSelectChr[5] + 1);
        mapper.bankChr1k(6, mmc5.bankSelectChr[7] + 0);
        mapper.bankChr1k(7, mmc5.bankSelectChr[7] + 1);
        break;
    case 3:
        mapper.bankChr1k(0, mmc5.bankSelectChr[0]);
        mapper.bankChr1k(1, mmc5.bankSelectChr[1]);
        mapper.bankChr1k(2, mmc5.bankSelectChr[2]);
        mapper.bankChr1k(3, mmc5.bankSelectChr[3]);
        mapper.bankChr1k(4, mmc5.bankSelectChr[4]);
        mapper.bankChr1k(5, mmc5.bankSelectChr[5]);
        mapper.bankChr1k(6, mmc5.bankSelectChr[6]);
        mapper.bankChr1k(7, mmc5.bankSelectChr[7]);
        break;
    }

    for (int i = 0; i < 8; ++i)
        mmc5.chr2[i] = mapper.chr(i);

    if (mmc5.mode8x16)
    {
        switch (mmc5.bankModeChr)
        {
        case 0:
            mapper.bankChr8k(mmc5.bankSelectChr[11]);
            break;
        case 1:
            mapper.bankChr4k(0, mmc5.bankSelectChr[11]);
            mapper.bankChr4k(1, mmc5.bankSelectChr[11]);
            break;
        case 2:
            mapper.bankChr2k(0, mmc5.bankSelectChr[9]);
            mapper.bankChr2k(1, mmc5.bankSelectChr[11]);
            mapper.bankChr2k(2, mmc5.bankSelectChr[9]);
            mapper.bankChr2k(3, mmc5.bankSelectChr[11]);
            break;
        case 3:
            mapper.bankChr1k(0, mmc5.bankSelectChr[8]);
            mapper.bankChr1k(1, mmc5.bankSelectChr[9]);
            mapper.bankChr1k(2, mmc5.bankSelectChr[10]);
            mapper.bankChr1k(3, mmc5.bankSelectChr[11]);
            mapper.bankChr1k(4, mmc5.bankSelectChr[8]);
            mapper.bankChr1k(5, mmc5.bankSelectChr[9]);
            mapper.bankChr1k(6, mmc5.bankSelectChr[10]);
            mapper.bankChr1k(7, mmc5.bankSelectChr[11]);
            break;
        }
    }
}

template <>
void Mapper::handleReset<MapperID::MMC5>()
{
    _mmc5                  = MapperMMC5{};
    _mmc5.bankModePrg      = 3;
    _mmc5.bankModeChr      = 3;
    _mmc5.bankSelectPrg[4] = 0xff;
    for (int i = 0; i < 12; ++i)
        _mmc5.bankSelectChr[i] = 0xff;
    _mmc5.mul[0] = 0xff;
    _mmc5.mul[1] = 0xff;

    applyChr(*this, _mmc5);
    applyPrg(*this, _mmc5);
}

template <>
void Mapper::handleTick<MapperID::MMC5>()
{
    if (_mmc5.ppuReading)
        _mmc5.ppuIdleCount = 0;
    else
    {
        _mmc5.ppuIdleCount++;
        // TODO: Fix this hack
        if (_mmc5.ppuIdleCount == 100)
        {
            _mmc5.inFrame = false;
        }
    }
    _mmc5.ppuReading = false;
}

template <>
std::uint8_t Mapper::handleRead<MapperID::MMC5>(std::uint16_t addr)
{
    std::uint8_t value{};

    switch (addr)
    {
    case 0x5204: // IRQ Control
        if (_mmc5.scanlinePending) value |= 0x80;
        if (_mmc5.inFrame) value |= 0x40;
        _mmc5.scanlinePending = false;
        _irq.unset(IRQ_MAPPER1);
        break;
    case 0x5205: // Mul Lo
        value = (_mmc5.mul[0] * _mmc5.mul[1]) & 0xff;
        break;
    case 0x5206: // Mul Hi
        value = (_mmc5.mul[0] * _mmc5.mul[1]) >> 8;
        break;
    case 0xfffa:
    case 0xfffb:
        _mmc5.inFrame         = false;
        _mmc5.scanline        = 0;
        _mmc5.scanlinePending = false;
        _irq.unset(IRQ_MAPPER1);
        break;
    }
    return value;
}

template <>
void Mapper::handleWrite<MapperID::MMC5>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x2000: // PPUCTRL
        _mmc5.ppuSpriteFlag = !!(value & 0x20);
        _mmc5.mode8x16      = (_mmc5.ppuRenderFlag && _mmc5.ppuSpriteFlag);
        applyChr(*this, _mmc5);
        break;
    case 0x2001: // PPUMASK
        _mmc5.ppuRenderFlag = !!(value & 0x18);
        _mmc5.mode8x16      = (_mmc5.ppuRenderFlag && _mmc5.ppuSpriteFlag);
        applyChr(*this, _mmc5);
        break;
    case 0x5100: // PRG Mode
        _mmc5.bankModePrg = value & 0x03;
        applyPrg(*this, _mmc5);
        break;
    case 0x5101: // CHR Mode
        _mmc5.bankModeChr = value & 0x03;
        applyChr(*this, _mmc5);
        break;
    case 0x5105: // Nametable Control
        _mmc5.nametable = value;
        break;
    case 0x5106: // Fill NT
        _mmc5.fillNT = value;
        break;
    case 0x5107: // Fill AT
        _mmc5.fillAT = value & 0x03;
        break;
    case 0x5113: // PRG Select
    case 0x5114:
    case 0x5115:
    case 0x5116:
    case 0x5117:
        _mmc5.bankSelectPrg[addr - 0x5113] = value;
        applyPrg(*this, _mmc5);
        break;
    case 0x5120: // CHR Select
    case 0x5121:
    case 0x5122:
    case 0x5123:
    case 0x5124:
    case 0x5125:
    case 0x5126:
    case 0x5127:
    case 0x5128:
    case 0x5129:
    case 0x512a:
    case 0x512b:
        _mmc5.bankSelectChr[addr - 0x5120] = value;
        applyChr(*this, _mmc5);
        break;
    case 0x5203: // IRQ Target
        _mmc5.scanlineTarget = value;
        break;
    case 0x5204: // IRQ Control
        _mmc5.scanlineEnabled = !!(value & 0x80);
        if (_mmc5.scanlineEnabled && _mmc5.scanlinePending)
            _irq.set(IRQ_MAPPER1);
        break;
    case 0x5205: // Mul 1
        _mmc5.mul[0] = value;
        break;
    case 0x5206: // Mul 2
        _mmc5.mul[1] = value;
        break;
    }
}

template <>
void Mapper::handleVideoRead<MapperID::MMC5>(std::uint16_t addr)
{
    _mmc5.ppuReading = true;

    if (_mmc5.ppuAddrCount == 0 && addr >= 0x2000 && addr < 0x3000)
    {
        _mmc5.ppuAddr      = addr;
        _mmc5.ppuAddrCount = 1;
    }
    else
    {
        if (addr != _mmc5.ppuAddr)
            _mmc5.ppuAddrCount = 0;
        else if (_mmc5.ppuAddrCount < 3)
        {
            _mmc5.ppuAddrCount++;
            if (_mmc5.ppuAddrCount == 3)
            {
                if (!_mmc5.inFrame)
                {
                    _mmc5.inFrame    = true;
                    _mmc5.scanline   = 0;
                    _mmc5.fetchCount = 1;
                    _irq.unset(IRQ_MAPPER1);
                }
                else
                {
                    _mmc5.scanline++;
                    _mmc5.fetchCount = 1;
                    if (_mmc5.scanline == 241)
                    {
                        _mmc5.inFrame         = false;
                        _mmc5.scanline        = 0;
                        _mmc5.scanlinePending = false;
                        _irq.unset(IRQ_MAPPER1);
                    }
                    else if (_mmc5.scanline == _mmc5.scanlineTarget)
                    {
                        _mmc5.scanlinePending = true;
                        if (_mmc5.scanlineEnabled)
                            _irq.set(IRQ_MAPPER1);
                    }
                }
            }
        }
    }
}

template <>
std::uint8_t Mapper::handleNtRead<MapperID::MMC5>(int nametable, std::uint16_t off)
{
    std::uint8_t value;

    if (off < 0x3c0)
        _mmc5.fetchCount++;

    switch ((_mmc5.nametable >> (nametable * 2)) & 0x03)
    {
    case 0:
        value = _memory.vram[off + 0x000];
        break;
    case 1:
        value = _memory.vram[off + 0x400];
        break;
    case 2:
        value = _memory.exvram[off];
        break;
    case 3:
        value = (off < 0x3c0) ? _mmc5.fillNT : _mmc5.fillAT;
        break;
    default:
        UNREACHABLE();
    }

    return value;
}

template <>
void Mapper::handleNtWrite<MapperID::MMC5>(int nametable, std::uint16_t off, std::uint8_t value)
{
    switch ((_mmc5.nametable >> (nametable * 2)) & 0x03)
    {
    case 0:
        _memory.vram[off + 0x000] = value;
        break;
    case 1:
        _memory.vram[off + 0x400] = value;
        break;
    case 2:
        _memory.exvram[off] = value;
        break;
    case 3:
        break;
    default:
        UNREACHABLE();
    }
}

template <>
std::uint8_t Mapper::handleChrRead<MapperID::MMC5>(int bank, std::uint16_t offset)
{
    if (_mmc5.mode8x16 && _mmc5.fetchCount == 33)
        return _mmc5.chr2[bank][offset];
    return _chr[bank][offset];
}

template <>
void Mapper::init<MapperID::MMC5>()
{
    _handleReset     = &Mapper::handleReset<MapperID::MMC5>;
    _handleTick      = &Mapper::handleTick<MapperID::MMC5>;
    _handleRead      = &Mapper::handleRead<MapperID::MMC5>;
    _handleWrite     = &Mapper::handleWrite<MapperID::MMC5>;
    _handleVideoRead = &Mapper::handleVideoRead<MapperID::MMC5>;
    _handleNtRead    = &Mapper::handleNtRead<MapperID::MMC5>;
    _handleNtWrite   = &Mapper::handleNtWrite<MapperID::MMC5>;
    _handleChrRead   = &Mapper::handleChrRead<MapperID::MMC5>;
}

}
