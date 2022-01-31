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
#include <libnin/Save.h>

using namespace libnin;

Save::Save(Cart& cart)
: _cart{cart}
, _file{}
, _battery{}
{
}

Save::~Save()
{
    sync();
    if (_file)
        std::fclose(_file);
}

void Save::setBattery(bool battery)
{
    _battery = battery;
}

void Save::setSaveFile(const char* path)
{
    if (!_battery)
        return;

    _file = std::fopen(path, "ab");
    if (_file)
        std::fclose(_file);

    _file = std::fopen(path, "r+b");
    if (_file)
    {
        _cart.load(CART_PRG_RAM, _cart.segment(CART_PRG_RAM).bankCount, _file);
    }
}

void Save::sync()
{
    const CartSegment& prgRam = _cart.segment(CART_PRG_RAM);

    if (!_file)
        return;

    std::fseek(_file, 0, SEEK_SET);
    std::fwrite(prgRam.base, std::size_t(prgRam.bankCount) * 0x2000, 1, _file);
    std::fflush(_file);
}
