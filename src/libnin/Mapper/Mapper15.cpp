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

#include <cstdlib>
#include <libnin/Cart.h>
#include <libnin/Mapper/Mapper15.h>
#include <libnin/Util.h>

namespace libnin
{

void Mapper15::handleInit()
{
    bankPrg32k(2, CART_PRG_ROM, 0);
    mirror(NIN_MIRROR_H);
}

void Mapper15::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        if (value & 0x40)
            mirror(NIN_MIRROR_V);
        else
            mirror(NIN_MIRROR_H);

        switch (addr & 3)
        {
        case 0:
            /* NROM-256 */
            bankPrg32k(2, CART_PRG_ROM, (value & 0x3f) >> 1);
            break;
        case 1:
            /* UNROM */
            bankPrg16k(2, CART_PRG_ROM, value & 0x3f);
            bankPrg16k(4, CART_PRG_ROM, (value & 0x3f) | 7);
            break;
        case 2:
            /* NROM-64 */
            bankPrg8k(2, CART_PRG_ROM, ((value & 0x3f) << 1) | (value >> 7));
            bankPrg8k(3, CART_PRG_ROM, ((value & 0x3f) << 1) | (value >> 7));
            bankPrg8k(4, CART_PRG_ROM, ((value & 0x3f) << 1) | (value >> 7));
            bankPrg8k(5, CART_PRG_ROM, ((value & 0x3f) << 1) | (value >> 7));
            break;
        case 3:
            /* NROM-128 */
            bankPrg16k(2, CART_PRG_ROM, (value & 0x3f));
            bankPrg16k(4, CART_PRG_ROM, (value & 0x3f));
            break;
        }
    }
}

} // namespace libnin
