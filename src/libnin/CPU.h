/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
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

#ifndef LIBNIN_CPU_H
#define LIBNIN_CPU_H 1

#include <cstddef>
#include <cstdint>
#include <libnin/MemberStateHelper.h>
#include <libnin/NonCopyable.h>

#define PFLAG_C 0x01
#define PFLAG_Z 0x02
#define PFLAG_I 0x04
#define PFLAG_D 0x08
#define PFLAG_B 0x10
#define PFLAG_1 0x20
#define PFLAG_V 0x40
#define PFLAG_N 0x80

#define PFLAG_MASK (~(PFLAG_B | PFLAG_1))

#define REG_A 0x00
#define REG_X 0x01
#define REG_Y 0x02
#define REG_S 0x03

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

    bool dispatching() const { return _handler == &CPU::dispatch; }

    std::uint8_t  reg(int r) const { return _regs[r]; }
    std::uint16_t pc() const { return _pc; }

    std::size_t tick(std::size_t cycles);

private:
    using Handler = MemberStateHelper<CPU>;

    template <int> Handler instruction(void);

    Handler dispatch();
    Handler kil();
    Handler dma();
    Handler dmaRead();
    Handler dmaWrite();

    std::uint8_t read(std::uint16_t addr);
    Handler      write(std::uint16_t addr, std::uint8_t value, Handler next);
    std::uint8_t adc(std::uint8_t a, std::uint8_t b);

    void flagNZ(std::uint8_t value)
    {
        _p &= ~(PFLAG_N | PFLAG_Z);
        _p |= (value & 0xff ? 0 : PFLAG_Z);
        _p |= (value & 0x80 ? PFLAG_N : 0);
    }

    static const Handler kOps[];
    static const Handler kStates[];

    Memory&  _memory;
    IRQ&     _irq;
    NMI&     _nmi;
    PPU&     _ppu;
    APU&     _apu;
    BusMain& _bus;

    Handler       _handler;
    Handler       _handler2;
    std::size_t   _cyc;
    std::uint16_t _pc;
    std::uint16_t _addr;
    union
    {
        std::uint8_t _regs[4];
        struct
        {
            std::uint8_t _a;
            std::uint8_t _x;
            std::uint8_t _y;
            std::uint8_t _s;
        };
    };
    std::uint8_t  _rmw;
    std::uint8_t  _addrCarry;
    std::uint8_t  _selSrc;
    std::uint8_t  _selDst;
    std::uint8_t  _p;
    std::uint8_t  _p2;
    std::uint16_t _dmaAddr;
    std::uint8_t  _dmaCount;
    std::uint8_t  _dmaValue;
    bool          _nmiPending : 1;
    bool          _irqPending : 1;
    bool          _odd : 1;
    bool          _reset : 1;
};

} // namespace libnin

#endif
