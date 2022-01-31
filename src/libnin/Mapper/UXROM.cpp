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
#include <libnin/Mapper/UXROM.h>
#include <libnin/Util.h>

namespace libnin
{

template <bool conflicts, int bank, int shift>
static void applyUxROM(MapperUXROM& mapper, std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        if (conflicts)
        {
            value &= mapper.bank(((addr - 0x8000) / 0x2000) + 2)[addr & 0x1fff];
        }
        mapper.bankPrg16k(bank, CART_PRG_ROM, value >> shift);
    }
}

void MapperUXROM::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    applyUxROM<true, 2, 0>(*this, addr, value);
}

void MapperUXROM::handleWrite_NoConflicts(std::uint16_t addr, std::uint8_t value)
{
    applyUxROM<false, 2, 0>(*this, addr, value);
}

void MapperUXROM::handleWrite_UN1ROM(std::uint16_t addr, std::uint8_t value)
{
    applyUxROM<true, 2, 2>(*this, addr, value);
}

void MapperUXROM::handleWrite_UNROM180(std::uint16_t addr, std::uint8_t value)
{
    applyUxROM<true, 4, 0>(*this, addr, value);
}

} // namespace libnin
