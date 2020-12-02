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
#include <libnin/Mapper/MMC2.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperMMC2::handleInit()
{
    _bankLatch0Lo = 0;
    _bankLatch0Hi = 0;
    _bankLatch1Lo = 0;
    _bankLatch1Hi = 0;
    _latch0       = 0;
    _latch1       = 0;

    bankPrg8k(3, CART_PRG_ROM, -3);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);

    applyChr();
}

void MapperMMC2::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        bankPrg8k(2, CART_PRG_ROM, value & 0xf);
        break;
    case 0xb000:
        _bankLatch0Lo = value & 0x1f;
        applyChr();
        break;
    case 0xc000:
        _bankLatch0Hi = value & 0x1f;
        applyChr();
        break;
    case 0xd000:
        _bankLatch1Lo = value & 0x1f;
        applyChr();
        break;
    case 0xe000:
        _bankLatch1Hi = value & 0x1f;
        applyChr();
        break;
    case 0xf000:
        if (value & 0x1)
            mirror(NIN_MIRROR_V);
        else
            mirror(NIN_MIRROR_H);
    }
}

void MapperMMC2::handleVideoRead(std::uint16_t addr)
{
    if (addr == 0x0fd8)
    {
        _latch0 = 0;
        applyChr();
    }
    else if (addr == 0x0fe8)
    {
        _latch0 = 1;
        applyChr();
    }
    else if (addr >= 0x1fd8 && addr <= 0x1fdf)
    {
        _latch1 = 0;
        applyChr();
    }
    else if (addr >= 0x1fe8 && addr <= 0x1fef)
    {
        _latch1 = 1;
        applyChr();
    }
}

void MapperMMC2::applyChr()
{
    bankChr4k(0, _latch0 ? _bankLatch0Hi : _bankLatch0Lo);
    bankChr4k(1, _latch1 ? _bankLatch1Hi : _bankLatch1Lo);
}

} // namespace libnin
