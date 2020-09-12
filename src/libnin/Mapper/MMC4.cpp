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
void Mapper::handleWrite<MapperID::MMC4>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        bankPrg16k(2, CART_PRG_ROM, value & 0xf);
        break;
    default:
        handleWrite<MapperID::MMC2>(addr, value);
        break;
    }
}

template <>
void Mapper::init<MapperID::MMC4>()
{
    init<MapperID::MMC2>();
    _handleWrite = &Mapper::handleWrite<MapperID::MMC4>;
}
