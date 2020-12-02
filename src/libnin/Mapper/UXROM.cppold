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

template <bool conflicts, int bank, int shift>
static void applyUxROM(Mapper& mapper, std::uint16_t addr, std::uint8_t value)
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

#define X(mapper, conflicts, bank, shift)                                                                                                                      \
    template <> void Mapper::handleWrite<MapperID::mapper>(std::uint16_t addr, std::uint8_t value) { applyUxROM<conflicts, bank, shift>(*this, addr, value); } \
    template <> void Mapper::init<MapperID::mapper>() { _handleWrite = &Mapper::handleWrite<MapperID::mapper>; }

X(UxROM, true, 2, 0);
X(UxROM_NoConflicts, false, 2, 0);
X(UxROM_UN1ROM, true, 2, 2);
X(UxROM_UNROM180, true, 4, 0);

} // namespace libnin
