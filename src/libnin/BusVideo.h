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

#ifndef LIBNIN_BUS_VIDEO_H
#define LIBNIN_BUS_VIDEO_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class Memory;
class Cart;
class Mapper;
class BusVideo : private NonCopyable
{
public:
    BusVideo(Memory& memory, Cart& cart, Mapper& mapper);

    std::uint8_t read(std::uint16_t addr);
    void         write(std::uint16_t addr, std::uint8_t value);

private:
    Memory& _memory;
    Cart&   _cart;
    Mapper& _mapper;
};

} // namespace libnin

#endif
