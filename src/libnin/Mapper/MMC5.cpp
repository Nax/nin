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
#include <libnin/Mapper/MMC5.h>
#include <libnin/Memory.h>
#include <libnin/Util.h>

using namespace libnin;

MapperMMC5::MapperMMC5(Memory& memory, Cart& cart, IRQ& irq)
: Mapper{memory, cart, irq}
, _bankModePrg{}
, _bankModeChr{}
, _bankSelectPrg{}
, _bankSelectChr{}
, _nametable{}
, _fillNT{}
, _fillAT{}
, _mul{}
, _chr2{}
, _ppuAddr{}
, _ppuAddrCount{}
, _ppuIdleCount{}
, _fetchCount{}
, _scanline{}
, _scanlineTarget{}
, _ppuReading{}
, _ppuSpriteFlag{}
, _ppuRenderFlag{}
, _mode8x16{}
, _inFrame{}
, _scanlineEnabled{}
, _scanlinePending{}
{

}

void MapperMMC5::handleReset()
{
    _bankModePrg = 3;
    _bankModeChr = 3;
    _bankSelectPrg[4] = 0xff;
    for (int i = 0; i < 12; ++i)
        _bankSelectChr[i] = 0xff;
    _mul[0] = 0xff;
    _mul[1] = 0xff;
    applyChr();
    applyPrg();
}

void MapperMMC5::handleTick(void)
{
    if (_ppuReading)
        _ppuIdleCount = 0;
    else
    {
        _ppuIdleCount++;
        // TODO: Fix this hack
        if (_ppuIdleCount == 100)
        {
            _inFrame = false;
        }
    }
    _ppuReading = false;
}

std::uint8_t MapperMMC5::handleRead(std::uint16_t addr)
{
    std::uint8_t value{};

    switch (addr)
    {
    case 0x5204: // IRQ Control
        if (_scanlinePending) value |= 0x80;
        if (_inFrame) value |= 0x40;
        _scanlinePending = false;
        _irq.unset(IRQ_MAPPER1);
        break;
    case 0x5205: // Mul Lo
        value = (_mul[0] * _mul[1]) & 0xff;
        break;
    case 0x5206: // Mul Hi
        value = (_mul[0] * _mul[1]) >> 8;
        break;
    case 0xfffa:
    case 0xfffb:
        _inFrame = false;
        _scanline = 0;
        _scanlinePending = false;
        _irq.unset(IRQ_MAPPER1);
        break;
    }
    return value;
}

void MapperMMC5::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x2000: // PPUCTRL
        _ppuSpriteFlag = !!(value & 0x20);
        _mode8x16 = (_ppuRenderFlag && _ppuSpriteFlag);
        applyChr();
        break;
    case 0x2001: // PPUMASK
        _ppuRenderFlag = !!(value & 0x18);
        _mode8x16 = (_ppuRenderFlag && _ppuSpriteFlag);
        applyChr();
        break;
    case 0x5100: // PRG Mode
        _bankModePrg = value & 0x03;
        applyPrg();
        break;
    case 0x5101: // CHR Mode
        _bankModeChr = value & 0x03;
        applyChr();
        break;
    case 0x5105: // Nametable Control
        _nametable = value;
        break;
    case 0x5106: // Fill NT
        _fillNT = value;
        break;
    case 0x5107: // Fill AT
        _fillAT = value & 0x03;
        break;
    case 0x5113: // PRG Select
    case 0x5114:
    case 0x5115:
    case 0x5116:
    case 0x5117:
        _bankSelectPrg[addr - 0x5113] = value;
        applyPrg();
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
        _bankSelectChr[addr - 0x5120] = value;
        applyChr();
        break;
    case 0x5203: // IRQ Target
        _scanlineTarget = value;
        break;
    case 0x5204: // IRQ Control
        _scanlineEnabled = !!(value & 0x80);
        if (_scanlineEnabled && _scanlinePending)
            _irq.set(IRQ_MAPPER1);
        break;
    case 0x5205: // Mul 1
        _mul[0] = value;
        break;
    case 0x5206: // Mul 2
        _mul[1] = value;
        break;
    }
}

void MapperMMC5::handleVideoRead(std::uint16_t addr)
{
    _ppuReading = true;

    if (_ppuAddrCount == 0 && addr >= 0x2000 && addr < 0x3000)
    {
        _ppuAddr = addr;
        _ppuAddrCount = 1;
    }
    else
    {
        if (addr != _ppuAddr)
            _ppuAddrCount = 0;
        else if (_ppuAddrCount < 3)
        {
            _ppuAddrCount++;
            if (_ppuAddrCount == 3)
            {
                if (!_inFrame)
                {
                    _inFrame = true;
                    _scanline = 0;
                    _fetchCount = 1;
                    _irq.unset(IRQ_MAPPER1);
                }
                else
                {
                    _scanline++;
                    _fetchCount = 1;
                    if (_scanline == 241)
                    {
                        _inFrame = false;
                        _scanline = 0;
                        _scanlinePending = false;
                        _irq.unset(IRQ_MAPPER1);
                    }
                    else if (_scanline == _scanlineTarget)
                    {
                        _scanlinePending = true;
                        if (_scanlineEnabled)
                            _irq.set(IRQ_MAPPER1);
                    }
                }
            }
        }
    }
}

std::uint8_t MapperMMC5::handleNtRead(int nametable, std::uint16_t off)
{
    std::uint8_t value;

    if (off < 0x3c0)
        _fetchCount++;

    switch ((_nametable >> (nametable * 2)) & 0x03)
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
        value = (off < 0x3c0) ? _fillNT : _fillAT;
        break;
    default:
        UNREACHABLE();
    }

    return value;
}

void MapperMMC5::handleNtWrite(int nametable, std::uint16_t off, std::uint8_t value)
{
    switch ((_nametable >> (nametable * 2)) & 0x03)
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

std::uint8_t MapperMMC5::handleChrRead(int bank, std::uint16_t offset)
{
    if (_mode8x16 && _fetchCount == 33)
        return _chr2[bank][offset];
    return _chr[bank][offset];
}

void MapperMMC5::applyPrg()
{
    bankPrg8k(1, CART_PRG_RAM, _bankSelectPrg[0]);
    switch (_bankModePrg)
    {
    case 0:
        bankPrg8k(2, CART_PRG_ROM, (_bankSelectPrg[4] & 0x7c) | 0x00);
        bankPrg8k(3, CART_PRG_ROM, (_bankSelectPrg[4] & 0x7c) | 0x01);
        bankPrg8k(4, CART_PRG_ROM, (_bankSelectPrg[4] & 0x7c) | 0x02);
        bankPrg8k(5, CART_PRG_ROM, (_bankSelectPrg[4] & 0x7c) | 0x03);
        break;
    case 1:
        bankPrg8k(2, (_bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_bankSelectPrg[2] & 0x7e) | 0x00);
        bankPrg8k(3, (_bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_bankSelectPrg[2] & 0x7e) | 0x01);
        bankPrg8k(4, CART_PRG_ROM, (_bankSelectPrg[4] & 0x7e) | 0x00);
        bankPrg8k(5, CART_PRG_ROM, (_bankSelectPrg[4] & 0x7e) | 0x01);
        break;
    case 2:
        bankPrg8k(2, (_bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_bankSelectPrg[2] & 0x7e) | 0x00);
        bankPrg8k(3, (_bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_bankSelectPrg[2] & 0x7e) | 0x01);
        bankPrg8k(4, (_bankSelectPrg[3] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _bankSelectPrg[3] & 0x7f);
        bankPrg8k(5, CART_PRG_ROM, _bankSelectPrg[4] & 0x7f);
        break;
    case 3:
        bankPrg8k(2, (_bankSelectPrg[1] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _bankSelectPrg[1] & 0x7f);
        bankPrg8k(3, (_bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _bankSelectPrg[2] & 0x7f);
        bankPrg8k(4, (_bankSelectPrg[3] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _bankSelectPrg[3] & 0x7f);
        bankPrg8k(5, CART_PRG_ROM, _bankSelectPrg[4] & 0x7f);
        break;
    }
}

void MapperMMC5::applyChr()
{
    const auto& seg = _cart.segment(CART_CHR_ROM);

    switch (_bankModeChr)
    {
    case 0:
        _chr2[0] = seg.base + std::uintptr_t((_bankSelectChr[7] + 0) % seg.bankCount) * 0x400;
        _chr2[1] = seg.base + std::uintptr_t((_bankSelectChr[7] + 1) % seg.bankCount) * 0x400;
        _chr2[2] = seg.base + std::uintptr_t((_bankSelectChr[7] + 2) % seg.bankCount) * 0x400;
        _chr2[3] = seg.base + std::uintptr_t((_bankSelectChr[7] + 3) % seg.bankCount) * 0x400;
        _chr2[4] = seg.base + std::uintptr_t((_bankSelectChr[7] + 4) % seg.bankCount) * 0x400;
        _chr2[5] = seg.base + std::uintptr_t((_bankSelectChr[7] + 5) % seg.bankCount) * 0x400;
        _chr2[6] = seg.base + std::uintptr_t((_bankSelectChr[7] + 6) % seg.bankCount) * 0x400;
        _chr2[7] = seg.base + std::uintptr_t((_bankSelectChr[7] + 7) % seg.bankCount) * 0x400;
        break;
    case 1:
        _chr2[0] = seg.base + std::uintptr_t((_bankSelectChr[3] + 0) % seg.bankCount) * 0x400;
        _chr2[1] = seg.base + std::uintptr_t((_bankSelectChr[3] + 1) % seg.bankCount) * 0x400;
        _chr2[2] = seg.base + std::uintptr_t((_bankSelectChr[3] + 2) % seg.bankCount) * 0x400;
        _chr2[3] = seg.base + std::uintptr_t((_bankSelectChr[3] + 3) % seg.bankCount) * 0x400;
        _chr2[4] = seg.base + std::uintptr_t((_bankSelectChr[7] + 0) % seg.bankCount) * 0x400;
        _chr2[5] = seg.base + std::uintptr_t((_bankSelectChr[7] + 1) % seg.bankCount) * 0x400;
        _chr2[6] = seg.base + std::uintptr_t((_bankSelectChr[7] + 2) % seg.bankCount) * 0x400;
        _chr2[7] = seg.base + std::uintptr_t((_bankSelectChr[7] + 3) % seg.bankCount) * 0x400;
        break;
    case 2:
        _chr2[0] = seg.base + std::uintptr_t((_bankSelectChr[1] + 0) % seg.bankCount) * 0x400;
        _chr2[1] = seg.base + std::uintptr_t((_bankSelectChr[1] + 1) % seg.bankCount) * 0x400;
        _chr2[2] = seg.base + std::uintptr_t((_bankSelectChr[3] + 0) % seg.bankCount) * 0x400;
        _chr2[3] = seg.base + std::uintptr_t((_bankSelectChr[3] + 1) % seg.bankCount) * 0x400;
        _chr2[4] = seg.base + std::uintptr_t((_bankSelectChr[5] + 0) % seg.bankCount) * 0x400;
        _chr2[5] = seg.base + std::uintptr_t((_bankSelectChr[5] + 1) % seg.bankCount) * 0x400;
        _chr2[6] = seg.base + std::uintptr_t((_bankSelectChr[7] + 0) % seg.bankCount) * 0x400;
        _chr2[7] = seg.base + std::uintptr_t((_bankSelectChr[7] + 1) % seg.bankCount) * 0x400;
        break;
    case 3:
        _chr2[0] = seg.base + std::uintptr_t(_bankSelectChr[0] % seg.bankCount) * 0x400;
        _chr2[1] = seg.base + std::uintptr_t(_bankSelectChr[1] % seg.bankCount) * 0x400;
        _chr2[2] = seg.base + std::uintptr_t(_bankSelectChr[2] % seg.bankCount) * 0x400;
        _chr2[3] = seg.base + std::uintptr_t(_bankSelectChr[3] % seg.bankCount) * 0x400;
        _chr2[4] = seg.base + std::uintptr_t(_bankSelectChr[4] % seg.bankCount) * 0x400;
        _chr2[5] = seg.base + std::uintptr_t(_bankSelectChr[5] % seg.bankCount) * 0x400;
        _chr2[6] = seg.base + std::uintptr_t(_bankSelectChr[6] % seg.bankCount) * 0x400;
        _chr2[7] = seg.base + std::uintptr_t(_bankSelectChr[7] % seg.bankCount) * 0x400;
        break;
    }

    if (_mode8x16)
    {
        switch (_bankModeChr)
        {
        case 0:
            bankChr8k(_bankSelectChr[11]);
            break;
        case 1:
            bankChr4k(0, _bankSelectChr[11]);
            bankChr4k(1, _bankSelectChr[11]);
            break;
        case 2:
            bankChr2k(0, _bankSelectChr[9]);
            bankChr2k(1, _bankSelectChr[11]);
            bankChr2k(2, _bankSelectChr[9]);
            bankChr2k(3, _bankSelectChr[11]);
            break;
        case 3:
            bankChr1k(0, _bankSelectChr[8]);
            bankChr1k(1, _bankSelectChr[9]);
            bankChr1k(2, _bankSelectChr[10]);
            bankChr1k(3, _bankSelectChr[11]);
            bankChr1k(4, _bankSelectChr[8]);
            bankChr1k(5, _bankSelectChr[9]);
            bankChr1k(6, _bankSelectChr[10]);
            bankChr1k(7, _bankSelectChr[11]);
            break;
        }
    }
    else
    {
        for (int i = 0; i < 8; ++i)
            _chr[i] = _chr2[i];
    }
}
