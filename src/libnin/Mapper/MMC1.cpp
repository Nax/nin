/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
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
#include <libnin/Mapper/MMC1.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperMMC1::handleInit()
{
    _shift       = 0;
    _count       = 0;
    _mirroring   = 0;
    _prgBankMode = 0;
    _chrBankMode = 0;
    _chrBank0    = 0;
    _chrBank1    = 0;
    _prgBank     = 0;

    applyMirror();
    applyPrg();
    applyChr();
}

void MapperMMC1::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    std::uint8_t shift;

    if (!(addr & 0x8000))
        return;

    if (value & 0x80)
    {
        /* Reset the internal registers */
        _count       = 0;
        _shift       = 0;
        _prgBankMode = 3;
        applyPrg();
        return;
    }

    shift = (((value & 1) << 4) | (_shift >> 1));
    if (_count < 4)
    {
        _shift = shift;
        _count++;
        return;
    }

    /* Serial write completed, apply the effect */
    _shift = 0;
    _count = 0;
    regWrite(addr, shift);
}

void MapperMMC1::regWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe000)
    {
    case 0x8000:
        /* 0x8000 - 0x9fff */
        _mirroring   = value & 0x03;
        _prgBankMode = (value >> 2) & 0x03;
        _chrBankMode = (value >> 4) & 0x01;

        applyMirror();
        applyPrg();
        applyChr();
        break;
    case 0xa000:
        /* 0xa000 - 0xbfff */
        _chrBank0 = value;
        applyChr();
        break;
    case 0xc000:
        /* 0xc000 - 0xdfff */
        _chrBank1 = value;
        applyChr();
        break;
    case 0xe000:
        /* 0xe000 - 0xffff */
        _prgBank = value & 0xf;
        applyPrg();
        break;
    }
}

void MapperMMC1::applyPrg()
{
    switch (_prgBankMode)
    {
    case 0:
    case 1:
        bankPrg16k(2, CART_PRG_ROM, (_prgBank & 0xfe) | 0);
        bankPrg16k(4, CART_PRG_ROM, (_prgBank & 0xfe) | 1);
        break;
    case 2:
        bankPrg16k(2, CART_PRG_ROM, 0);
        bankPrg16k(4, CART_PRG_ROM, _prgBank & 0xff);
        break;
    case 3:
        bankPrg16k(2, CART_PRG_ROM, _prgBank & 0xff);
        bankPrg16k(4, CART_PRG_ROM, -1);
        break;
    }
}

void MapperMMC1::applyChr()
{
    if (_chrBankMode)
    {
        bankChr4k(0, _chrBank0);
        bankChr4k(1, _chrBank1);
    }
    else
    {
        bankChr4k(0, _chrBank0 & ~(0x01));
        bankChr4k(1, _chrBank0 | 0x01);
    }
}

void MapperMMC1::applyMirror()
{
    int mirrorMode;

    switch (_mirroring)
    {
    case 0x00:
        mirrorMode = NIN_MIRROR_A;
        break;
    case 0x01:
        mirrorMode = NIN_MIRROR_B;
        break;
    case 0x02:
        mirrorMode = NIN_MIRROR_H;
        break;
    case 0x03:
        mirrorMode = NIN_MIRROR_V;
        break;
    default:
        UNREACHABLE();
    }
    mirror(mirrorMode);
}

} // namespace libnin
