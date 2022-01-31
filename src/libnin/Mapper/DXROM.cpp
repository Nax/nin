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
#include <libnin/Mapper/DXROM.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperDXROM::handleInit()
{
    _bankSelect = 0;

    bankPrg8k(2, CART_PRG_ROM, 0);
    bankPrg8k(3, CART_PRG_ROM, 0);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);
}

void MapperDXROM::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000 && addr <= 0x9fff)
    {
        if ((addr & 1) == 0)
            _bankSelect = value & 7;
        else
        {
            switch (_bankSelect)
            {
            case 0:
                bankChr2k(0, (value & 0x3e) >> 1);
                break;
            case 1:
                bankChr2k(1, (value & 0x3e) >> 1);
                break;
            case 2:
                bankChr1k(4, value & 0x3f);
                break;
            case 3:
                bankChr1k(5, value & 0x3f);
                break;
            case 4:
                bankChr1k(6, value & 0x3f);
                break;
            case 5:
                bankChr1k(7, value & 0x3f);
                break;
            case 6:
                bankPrg8k(2, CART_PRG_ROM, value & 0x0f);
                break;
            case 7:
                bankPrg8k(3, CART_PRG_ROM, value & 0x0f);
                break;
            }
        }
    }
}

} // namespace libnin
