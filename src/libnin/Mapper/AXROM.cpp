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

using namespace libnin;

template <>
void Mapper::handleWrite<MapperID::AxROM>(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        mirror(((value >> 4) & 1) ? NIN_MIRROR_B : NIN_MIRROR_A);
        bankPrg32k(2, CART_PRG_ROM, value & 0xf);
    }
}

template <>
void Mapper::handleWrite<MapperID::AxROM_Conflicts>(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        value &= _prg[(addr - 0x8000) / 0x2000 + 2][addr & 0x1fff];
        mirror(((value >> 4) & 1) ? NIN_MIRROR_B : NIN_MIRROR_A);
        bankPrg32k(2, CART_PRG_ROM, value & 0xf);
    }
}

template <>
void Mapper::init<MapperID::AxROM>()
{
    _handleWrite = &Mapper::handleWrite<MapperID::AxROM>;
}

template <>
void Mapper::init<MapperID::AxROM_Conflicts>()
{
    _handleWrite = &Mapper::handleWrite<MapperID::AxROM_Conflicts>;
}
