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

#ifndef LIBNIN_BUS_MAIN_H
#define LIBNIN_BUS_MAIN_H 1

#include <cstdlib>
#include <libnin/NonCopyable.h>

namespace libnin
{

enum class WriteAction
{
    None,
    DMA
};

class Memory;
class Cart;
class Mapper;
class PPU;
class APU;
class Input;
class BusMain : private NonCopyable
{
public:
    BusMain(Memory& memory, Cart& cart, Mapper& mapper, PPU& ppu, APU& apu, Input& input);

    std::uint8_t read(std::uint16_t addr);
    WriteAction  write(std::uint16_t addr, std::uint8_t value);

    void dump(std::uint8_t* dst, std::uint16_t start, std::size_t len);

private:
    Memory& _memory;
    Cart&   _cart;
    Mapper& _mapper;
    PPU&    _ppu;
    APU&    _apu;
    Input&  _input;
};

}; // namespace libnin

#endif
