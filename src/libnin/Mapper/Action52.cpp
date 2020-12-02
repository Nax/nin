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
#include <libnin/Mapper/Action52.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperAction52::handleInit()
{
    bankPrg32k(2, CART_PRG_ROM, 0);
    bankChr8k(0);
}

void MapperAction52::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    std::uint16_t prgBank;
    std::uint16_t chrBank;

    if (addr >= 0x8000)
    {
        chrBank = ((addr & 0x000f) << 2) | (value & 0x03);
        prgBank = (addr >> 6) & 0x1f;
        switch ((addr >> 0xb) & 0x3)
        {
        case 0:
            prgBank += 0;
            break;
        case 1:
            prgBank += 32;
            break;
        case 2:
            break;
        case 3:
            prgBank += 64;
        }

        bankChr8k(chrBank);
        if (addr & 0x20)
        {
            bankPrg16k(2, CART_PRG_ROM, prgBank);
            bankPrg16k(4, CART_PRG_ROM, prgBank);
        }
        else
        {
            bankPrg16k(2, CART_PRG_ROM, (prgBank & 0xfffe) | 0);
            bankPrg16k(4, CART_PRG_ROM, (prgBank & 0xfffe) | 1);
        }
        mirror(addr & 0x2000 ? NIN_MIRROR_V : NIN_MIRROR_H);
    }
}

} // namespace libnin
