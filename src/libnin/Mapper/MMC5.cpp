/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 *
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 *
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 *
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <libnin/Cart.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper/MMC5.h>
#include <libnin/Memory.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperMMC5::handleInit()
{
    _bankModePrg       = 3;
    _bankModeChr       = 3;
    _bankSelectPrg[0]  = 0;
    _bankSelectPrg[1]  = 0;
    _bankSelectPrg[2]  = 0;
    _bankSelectPrg[3]  = 0;
    _bankSelectPrg[4]  = 0xff;
    _bankSelectChr[0]  = 0xff;
    _bankSelectChr[1]  = 0xff;
    _bankSelectChr[2]  = 0xff;
    _bankSelectChr[3]  = 0xff;
    _bankSelectChr[4]  = 0xff;
    _bankSelectChr[5]  = 0xff;
    _bankSelectChr[6]  = 0xff;
    _bankSelectChr[7]  = 0xff;
    _bankSelectChr[8]  = 0xff;
    _bankSelectChr[9]  = 0xff;
    _bankSelectChr[10] = 0xff;
    _bankSelectChr[11] = 0xff;
    _nametable         = 0;
    _fillNT            = 0;
    _fillAT            = 0;
    _mul[0]            = 0xff;
    _mul[1]            = 0xff;
    _chr2[0]           = nullptr;
    _chr2[1]           = nullptr;
    _chr2[2]           = nullptr;
    _chr2[3]           = nullptr;
    _chr2[4]           = nullptr;
    _chr2[5]           = nullptr;
    _chr2[6]           = nullptr;
    _chr2[7]           = nullptr;
    _ppuAddr           = 0;
    _ppuAddrCount      = 0;
    _ppuIdleCount      = 0;
    _fetchCount        = 0;
    _scanline          = 0;
    _scanlineTarget    = 0;
    _ppuReading        = false;
    _ppuSpriteFlag     = false;
    _ppuRenderFlag     = false;
    _mode8x16          = false;
    _inFrame           = false;
    _scanlineEnabled   = false;
    _scanlinePending   = false;

    applyChr();
    applyPrg();
}

void MapperMMC5::handleTick()
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
        _inFrame         = false;
        _scanline        = 0;
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
        _mode8x16      = (_ppuRenderFlag && _ppuSpriteFlag);
        applyChr();
        break;
    case 0x2001: // PPUMASK
        _ppuRenderFlag = !!(value & 0x18);
        _mode8x16      = (_ppuRenderFlag && _ppuSpriteFlag);
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
        _ppuAddr      = addr;
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
                    _inFrame    = true;
                    _scanline   = 0;
                    _fetchCount = 1;
                    _irq.unset(IRQ_MAPPER1);
                }
                else
                {
                    _scanline++;
                    _fetchCount = 1;
                    if (_scanline == 241)
                    {
                        _inFrame         = false;
                        _scanline        = 0;
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
    switch (_bankModeChr)
    {
    case 0:
        bankChr1k(0, _bankSelectChr[7] + 0);
        bankChr1k(1, _bankSelectChr[7] + 1);
        bankChr1k(2, _bankSelectChr[7] + 2);
        bankChr1k(3, _bankSelectChr[7] + 3);
        bankChr1k(4, _bankSelectChr[7] + 4);
        bankChr1k(5, _bankSelectChr[7] + 5);
        bankChr1k(6, _bankSelectChr[7] + 6);
        bankChr1k(7, _bankSelectChr[7] + 7);
        break;
    case 1:
        bankChr1k(0, _bankSelectChr[3] + 0);
        bankChr1k(1, _bankSelectChr[3] + 1);
        bankChr1k(2, _bankSelectChr[3] + 2);
        bankChr1k(3, _bankSelectChr[3] + 3);
        bankChr1k(4, _bankSelectChr[7] + 0);
        bankChr1k(5, _bankSelectChr[7] + 1);
        bankChr1k(6, _bankSelectChr[7] + 2);
        bankChr1k(7, _bankSelectChr[7] + 3);
        break;
    case 2:
        bankChr1k(0, _bankSelectChr[1] + 0);
        bankChr1k(1, _bankSelectChr[1] + 1);
        bankChr1k(2, _bankSelectChr[3] + 0);
        bankChr1k(3, _bankSelectChr[3] + 1);
        bankChr1k(4, _bankSelectChr[5] + 0);
        bankChr1k(5, _bankSelectChr[5] + 1);
        bankChr1k(6, _bankSelectChr[7] + 0);
        bankChr1k(7, _bankSelectChr[7] + 1);
        break;
    case 3:
        bankChr1k(0, _bankSelectChr[0]);
        bankChr1k(1, _bankSelectChr[1]);
        bankChr1k(2, _bankSelectChr[2]);
        bankChr1k(3, _bankSelectChr[3]);
        bankChr1k(4, _bankSelectChr[4]);
        bankChr1k(5, _bankSelectChr[5]);
        bankChr1k(6, _bankSelectChr[6]);
        bankChr1k(7, _bankSelectChr[7]);
        break;
    }

    for (int i = 0; i < 8; ++i)
        _chr2[i] = chr(i);

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
}

} // namespace libnin
