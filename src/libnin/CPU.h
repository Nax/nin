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

#ifndef LIBNIN_CPU_H
#define LIBNIN_CPU_H 1

#include <cstddef>
#include <cstdint>
#include <libnin/NonCopyable.h>

#define PFLAG_C     0x01
#define PFLAG_Z     0x02
#define PFLAG_I     0x04
#define PFLAG_D     0x08
#define PFLAG_B     0x10
#define PFLAG_1     0x20
#define PFLAG_V     0x40
#define PFLAG_N     0x80

#define PFLAG_MASK  (~(PFLAG_B | PFLAG_1))

#define REG_A   0x00
#define REG_X   0x01
#define REG_Y   0x02
#define REG_S   0x03

namespace libnin
{

class IRQ;
class NMI;
class PPU;
class APU;
class BusMain;
class CPU : private NonCopyable
{
public:
    CPU(IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus);

    std::size_t tick(std::size_t cycles);

private:
    template <std::uint16_t N>
    void instruction(std::uint8_t tmp);

    void            flagZ(std::uint8_t value);
    void            flagN(std::uint8_t value);
    std::uint8_t    compare(std::uint8_t a, std::uint8_t b);
    void            stackPush8(std::uint8_t value);
    std::uint8_t    stackPop8();
    std::uint8_t    asl(std::uint8_t v);
    std::uint8_t    rol(std::uint8_t v);
    std::uint8_t    lsr(std::uint8_t v);
    std::uint8_t    ror(std::uint8_t v);
    std::uint8_t    bit(std::uint8_t v);
    std::uint8_t    adc(std::uint8_t a, std::uint8_t b);
    std::uint16_t   indirect(std::uint16_t addr);

    IRQ&        _irq;
    NMI&        _nmi;
    PPU&        _ppu;
    APU&        _apu;
    BusMain&    _bus;

    std::size_t     _cyc;
    std::uint16_t   _pc;
    std::uint8_t    _regs[4];
    std::uint8_t    _p;
    std::uint8_t    _p2;
    bool            _nmi2:1;
    bool            _reset:1;
};

}

#endif
