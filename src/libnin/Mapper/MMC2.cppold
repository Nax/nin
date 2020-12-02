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
#include <libnin/Mapper.h>
#include <libnin/Util.h>

namespace libnin
{

static void apply(Mapper& mapper, MapperMMC2& mmc2)
{
    mapper.bankChr4k(0, mmc2.latch0 ? mmc2.bankLatch0Hi : mmc2.bankLatch0Lo);
    mapper.bankChr4k(1, mmc2.latch1 ? mmc2.bankLatch1Hi : mmc2.bankLatch1Lo);
}

template <>
void Mapper::handleReset<MapperID::MMC2>()
{
    _mmc2 = MapperMMC2{};

    bankPrg8k(3, CART_PRG_ROM, -3);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);
}

template <>
void Mapper::handleWrite<MapperID::MMC2>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        bankPrg8k(2, CART_PRG_ROM, value & 0xf);
        break;
    case 0xb000:
        _mmc2.bankLatch0Lo = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xc000:
        _mmc2.bankLatch0Hi = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xd000:
        _mmc2.bankLatch1Lo = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xe000:
        _mmc2.bankLatch1Hi = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xf000:
        if (value & 0x1)
            mirror(NIN_MIRROR_V);
        else
            mirror(NIN_MIRROR_H);
    }
}

template <>
void Mapper::handleVideoRead<MapperID::MMC2>(std::uint16_t addr)
{
    if (addr == 0x0fd8)
    {
        _mmc2.latch0 = 0;
        apply(*this, _mmc2);
    }
    else if (addr == 0x0fe8)
    {
        _mmc2.latch0 = 1;
        apply(*this, _mmc2);
    }
    else if (addr >= 0x1fd8 && addr <= 0x1fdf)
    {
        _mmc2.latch1 = 0;
        apply(*this, _mmc2);
    }
    else if (addr >= 0x1fe8 && addr <= 0x1fef)
    {
        _mmc2.latch1 = 1;
        apply(*this, _mmc2);
    }
}

template <>
void Mapper::init<MapperID::MMC2>()
{
    _handleReset     = &Mapper::handleReset<MapperID::MMC2>;
    _handleWrite     = &Mapper::handleWrite<MapperID::MMC2>;
    _handleVideoRead = &Mapper::handleVideoRead<MapperID::MMC2>;
}

}
