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

    std::uint8_t* prg(int i) { return _prg[i]; }
    std::uint8_t* chr(int i) { return _chr[i]; }
    std::uint8_t* nametable(int i) { return _nametables[i]; }

    void configure(std::uint16_t mapperMajor, std::uint8_t mapperMinor);
    void mirror(int mirrorMode);
    void write(std::uint16_t addr, std::uint8_t value) { (this->*_writeHandler)(addr, value); }

    void bankPrg8k(std::uint8_t slot, std::int16_t bank);
    void bankPrg16k(std::uint8_t slot, std::int16_t bank);
    void bankPrg32k(std::int16_t bank);

    void bankChr1k(std::uint8_t slot, std::int16_t bank);
    void bankChr4k(std::uint8_t slot, std::int16_t bank);
    void bankChr8k(std::int16_t bank);

private:
    using WriteHandler = void(Mapper::*)(std::uint16_t, std::uint8_t);

    struct MMC1
    {
        std::uint8_t shift;
        std::uint8_t count;
        std::uint8_t mirroring:2;
        std::uint8_t prgBankMode:2;
        std::uint8_t chrBankMode:1;
        std::uint8_t chrBank0:5;
        std::uint8_t chrBank1:5;
        std::uint8_t prgBank:4;
    };

    void write_NROM(std::uint16_t addr, std::uint8_t value);
    void write_UXROM(std::uint16_t addr, std::uint8_t value);
    void write_UXROM180(std::uint16_t addr, std::uint8_t value);
    void write_CNROM(std::uint16_t addr, std::uint8_t value);
    void write_GXROM(std::uint16_t addr, std::uint8_t value);
    void write_AXROM(std::uint16_t addr, std::uint8_t value);
    void write_ColorDreams(std::uint16_t addr, std::uint8_t value);
    void write_MMC1(std::uint16_t addr, std::uint8_t value);

    void mmc1RegWrite(std::uint16_t addr, std::uint8_t value);
    void mmc1BankPrg();
    void mmc1BankChr();
    void mmc1Mirror();

    Memory&     _memory;
    Cart&       _cart;

    WriteHandler    _writeHandler;
    std::uint8_t*   _prg[4];
    std::uint8_t*   _chr[8];
    std::uint8_t*   _nametables[4];

    union
    {
        MMC1 _mmc1;
    };
};

}

#endif
