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

#ifndef LIBNIN_MAPPER_H
#define LIBNIN_MAPPER_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

#define NIN_MIRROR_A 0
#define NIN_MIRROR_B 1
#define NIN_MIRROR_H 2
#define NIN_MIRROR_V 3

namespace libnin
{

class Memory;
class Cart;
class Mapper : private NonCopyable
{
public:
    Mapper(Memory& memory, Cart& cart);
    ~Mapper();

    std::uint8_t* nametable(int i) { return _nametables[i]; }

    void configure(std::uint16_t mapperMajor, std::uint8_t mapperMinor);
    void mirror(int mirrorMode);
    void write(std::uint16_t addr, std::uint8_t value) { (this->*_writeHandler)(addr, value); }

private:
    using WriteHandler = void(Mapper::*)(std::uint16_t, std::uint8_t);

    void write_NROM(std::uint16_t addr, std::uint8_t value);

    Memory&     _memory;
    Cart&       _cart;

    WriteHandler    _writeHandler;
    std::uint8_t*   _nametables[4];
};

}

#endif
