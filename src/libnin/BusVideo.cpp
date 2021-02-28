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

#include <libnin/BusVideo.h>
#include <libnin/Cart.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>

using namespace libnin;

static std::uint8_t badIO(std::uint16_t addr, int write)
{
    printf("PPU bad IO at 0x%04x (%c)\n", addr, write ? 'w' : 'r');
    fflush(stdout);
    return 0;
}

BusVideo::BusVideo(Memory& memory, Cart& cart, Mapper& mapper)
: _memory(memory)
, _cart(cart)
, _mapper(mapper)
{
}

std::uint8_t BusVideo::read(std::uint16_t addr)
{
    addr &= 0x3fff;
    _mapper.videoRead(addr);
    if (addr < 0x2000)
        return _mapper.chrRead(addr / 0x400, addr & 0x3ff);
    else if (addr < 0x2400)
        return _mapper.ntRead(0, addr & 0x3ff);
    else if (addr < 0x2800)
        return _mapper.ntRead(1, addr & 0x3ff);
    else if (addr < 0x2c00)
        return _mapper.ntRead(2, addr & 0x3ff);
    else if (addr < 0x3000)
        return _mapper.ntRead(3, addr & 0x3ff);
    else if (addr < 0x3400)
        return _mapper.ntRead(0, addr & 0x3ff);
    else if (addr < 0x3800)
        return _mapper.ntRead(1, addr & 0x3ff);
    else if (addr < 0x3c00)
        return _mapper.ntRead(2, addr & 0x3ff);
    else if (addr < 0x3f00)
        return _mapper.ntRead(3, addr & 0x3ff);
    else
        return _memory.palettes[addr & 0x1f];
}

void BusVideo::write(std::uint16_t addr, std::uint8_t value)
{
    addr &= 0x3fff;
    _mapper.videoRead(addr);
    if (addr < 0x2000)
        _mapper.chrWrite(addr / 0x400, addr & 0x3ff, value);
    else if (addr < 0x2400)
        _mapper.ntWrite(0, addr & 0x3ff, value);
    else if (addr < 0x2800)
        _mapper.ntWrite(1, addr & 0x3ff, value);
    else if (addr < 0x2c00)
        _mapper.ntWrite(2, addr & 0x3ff, value);
    else if (addr < 0x3000)
        _mapper.ntWrite(3, addr & 0x3ff, value);
    else if (addr < 0x3400)
        _mapper.ntWrite(0, addr & 0x3ff, value);
    else if (addr < 0x3800)
        _mapper.ntWrite(1, addr & 0x3ff, value);
    else if (addr < 0x3c00)
        _mapper.ntWrite(2, addr & 0x3ff, value);
    else if (addr < 0x3f00)
        _mapper.ntWrite(3, addr & 0x3ff, value);
    else
    {
        if ((addr & 0x03) == 0)
        {
            _memory.palettes[addr & 0x0f]          = value;
            _memory.palettes[0x10 | (addr & 0x0f)] = value;
        }
        else
            _memory.palettes[addr & 0x1f] = value;
    }
}
