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

class Memory;
class IRQ;
class NMI;
class PPU;
class APU;
class BusMain;
class CPU : private NonCopyable
{
public:
    CPU(Memory& memory, IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus);

    std::uint8_t    reg(int r) const { return _regs[r]; }
    std::uint16_t   pc() const { return _pc; }

    std::size_t tick(std::size_t cycles);

private:
    using AnyFuncPtr = void* (CPU::*)(void);
    using Handler = AnyFuncPtr (CPU::*)(void);

    template <std::uint16_t> Handler instruction(void);

    Handler dispatch();
    Handler debug_not_impl(std::uint16_t);

    std::uint8_t    read(std::uint16_t addr);
    void            write(std::uint16_t addr, uint8_t value);

    void flagNZ(std::uint8_t value)
    {
        _p &= ~(PFLAG_N | PFLAG_Z);
        _p |= (value & 0xff ? 0 : PFLAG_Z);
        _p |= (value & 0x80 ? PFLAG_N : 0);
    }

    static const Handler kStates[];

    Memory&     _memory;
    IRQ&        _irq;
    NMI&        _nmi;
    PPU&        _ppu;
    APU&        _apu;
    BusMain&    _bus;

    Handler         _handler;
    std::size_t     _cyc;
    std::uint16_t   _pc;
    std::uint16_t   _addr;
    union
    {
        std::uint8_t    _regs[4];
        struct
        {
            std::uint8_t    _a;
            std::uint8_t    _x;
            std::uint8_t    _y;
            std::uint8_t    _s;
        };
    };
    std::uint8_t    _addrCarry;
    std::uint8_t    _selSrc;
    std::uint8_t    _selDst;
    std::uint8_t    _p;
    std::uint8_t    _p2;
    bool            _nmi2:1;
    bool            _reset:1;
};

}

#endif
