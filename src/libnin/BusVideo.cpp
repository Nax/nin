/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
            _memory.palettes[addr & 0x0f] = value;
            _memory.palettes[0x10 | (addr & 0x0f)] = value;
        }
        else
            _memory.palettes[addr & 0x1f] = value;
    }
}
