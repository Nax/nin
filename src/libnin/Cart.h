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

#ifndef LIBNIN_CART_H
#define LIBNIN_CART_H 1

#include <cstdint>
#include <cstdio>
#include <libnin/NonCopyable.h>

#define CART_PRG_ROM 0
#define CART_PRG_RAM 1
#define CART_CHR_ROM 2
#define CART_CHR_RAM 3

namespace libnin
{

struct CartSegment : private NonCopyable
{
    CartSegment()
    : base{}
    , bankCount{}
    {
    }
    ~CartSegment() { delete[] base; }

    std::uint8_t* base;
    std::uint16_t bankCount;
};

class Cart : private NonCopyable
{
public:
    const CartSegment& segment(int id) const { return _segments[id]; };

    void load(int id, std::uint16_t bankCount, std::FILE* file);

private:
    CartSegment _segments[4];
};

}; // namespace libnin

#endif
