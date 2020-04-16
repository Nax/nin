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
class IRQ;
class Mapper : private NonCopyable
{
public:
    Mapper(Memory& memory, Cart& cart, IRQ& irq);
    virtual ~Mapper();

    static Mapper* create(Memory& memory, Cart& cart, IRQ& irq, int major, int minor);

    const std::uint8_t* bank(int slot) const;

    std::uint8_t    read(std::uint16_t addr);
    void            write(std::uint16_t addr, std::uint8_t value);

    void mirror(int mirrorMode);

    void bankPrg8k(std::uint8_t slot, int domain, std::int16_t bank);
    void bankPrg16k(std::uint8_t slot, int domain, std::int16_t bank);
    void bankPrg32k(std::uint8_t slot, int domain, std::int16_t bank);

    void bankChr1k(std::uint8_t slot, std::int16_t bank);
    void bankChr2k(std::uint8_t slot, std::int16_t bank);
    void bankChr4k(std::uint8_t slot, std::int16_t bank);
    void bankChr8k(std::int16_t bank);

    virtual void            handleReset();
    virtual void            handleTick();
    virtual std::uint8_t    handleRead(std::uint16_t addr);
    virtual void            handleWrite(std::uint16_t addr, std::uint8_t value);
    virtual void            handleVideoRead(std::uint16_t addr);
    virtual std::uint8_t    handleNtRead(int nametable, std::uint16_t offset);
    virtual void            handleNtWrite(int nametable, std::uint16_t offset, std::uint8_t value);
    virtual std::uint8_t    handleChrRead(int bank, std::uint16_t offset);
    virtual void            handleChrWrite(int bank, std::uint16_t offset, std::uint8_t value);

protected:
    Memory& _memory;
    Cart&   _cart;
    IRQ&    _irq;

    std::uint8_t*   _prg[6];
    bool            _prgWriteFlag[6];
    std::uint8_t*   _chr[8];
    std::uint8_t*   _nametables[4];
};

}

#endif
