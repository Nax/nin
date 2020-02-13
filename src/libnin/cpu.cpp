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

#include <libnin/APU.h>
#include <libnin/BusMain.h>
#include <libnin/CPU.h>
#include <libnin/IRQ.h>
#include <libnin/NMI.h>
#include <libnin/PPU.h>

using namespace libnin;

CPU::CPU(IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus)
: _irq{ irq }
, _nmi{ nmi }
, _ppu{ ppu }
, _apu{ apu }
, _bus{ bus }
, _handler{&CPU::reset<0>}
, _pc{}
, _regs{}
, _p{}
, _p2{}
, _nmi2{}
, _reset{ true }
{

}

std::size_t CPU::tick(std::size_t cycles)
{
    uint8_t tmp;
    uint16_t op;
    bool isIRQ;

    _cyc = 0;
    if (_reset)
    {
        _p = PFLAG_I;
        _pc = _bus.read16(0xfffc);
        _regs[REG_S] = 0xfd;
        _reset = false;
    }

    for (;;)
    {
        isIRQ = (!(_p & PFLAG_I) && _irq.high());

        op = _bus.read(_pc);
        CYCLE();
        tmp = _bus.read(_pc + 1);
        CYCLE();

        if (_nmi2 || isIRQ)
        {
            if (isIRQ)
            {
                op = 0x100;
            }
            else
            {
                _nmi.ack();
                op = 0x101;
            }
        }
        else
        {
            _pc++;
        }

        _nmi2 = _nmi.high();
        //_p2 = _p;

        EXECUTE(op);

        if (_cyc >= cycles)
            break;
    }
    return _cyc - cycles;
}

template <std::uint16_t op>
CPU::Handler CPU::instruction()
{

}

template <std::uint8_t step>
CPU::Handler CPU::decode()
{

}

template <std::uint8_t step>
CPU::Handler CPU::reset()
{

}

void CPU::flagZ(std::uint8_t value)
{
    _p &= ~PFLAG_Z;
    _p |= ((!value) << 1);
}

void CPU::flagN(std::uint8_t value)
{
    _p &= ~PFLAG_N;
    if (value & 0x80)
        _p |= PFLAG_N;
}

std::uint8_t CPU::compare(std::uint8_t a, std::uint8_t b)
{
    _p &= ~(PFLAG_C);
    if (a >= b)
        _p |= PFLAG_C;
    return a - b;
}

void CPU::stackPush8(std::uint8_t value)
{
    _bus.write(0x100 | _regs[REG_S], value);
    _regs[REG_S]--;
}

std::uint8_t CPU::stackPop8()
{
    _regs[REG_S]++;
    return _bus.read(0x100 | _regs[REG_S]);
}

std::uint8_t CPU::asl(std::uint8_t v)
{
    _p &= ~PFLAG_C;
    if (v & 0x80) _p |= PFLAG_C;

    return (v << 1);
}

std::uint8_t CPU::rol(std::uint8_t v)
{
    std::uint8_t carry;

    carry = (_p & PFLAG_C) ? 0x01 : 0x00;
    _p &= ~PFLAG_C;
    if (v & 0x80) _p |= PFLAG_C;

    return ((v << 1) | carry);
}

std::uint8_t CPU::lsr(std::uint8_t v)
{
    _p &= ~PFLAG_C;
    if (v & 0x01) _p |= PFLAG_C;

    return (v >> 1);
}

std::uint8_t CPU::ror(std::uint8_t v)
{
    std::uint8_t carry;

    carry = (_p & PFLAG_C) ? 0x80 : 0x00;
    _p &= ~PFLAG_C;
    if (v & 0x01) _p |= PFLAG_C;

    return ((v >> 1) | carry);
}

std::uint8_t CPU::bit(std::uint8_t v)
{
    _p &= ~(0xc0);
    _p |= (v & 0xc0);

    return _regs[REG_A] & v;
}

std::uint8_t CPU::adc(std::uint8_t a, std::uint8_t b)
{
    std::uint16_t sum;
    std::uint8_t carryIn;

    carryIn = (_p & PFLAG_C) ? 1 : 0;
    sum = (std::uint16_t)a + (std::uint16_t)b + (std::uint16_t)carryIn;

    _p &= ~(PFLAG_C | PFLAG_V | PFLAG_N | PFLAG_Z);
    if (sum & 0x100) _p |= PFLAG_C;
    if ((sum & 0xff) == 0) _p |= PFLAG_Z;
    if ((a ^ sum) & (b ^ sum) & 0x80) _p |= PFLAG_V;
    if (sum & 0x80) _p |= PFLAG_N;

    return (sum & 0xff);
}

std::uint16_t CPU::indirect(std::uint16_t addr)
{
    std::uint8_t hi;
    std::uint8_t lo0;
    std::uint8_t lo1;
    std::uint8_t hh;
    std::uint8_t ll;

    hi = addr >> 8;
    lo0 = addr & 0xff;
    lo1 = lo0 + 1;

    ll = _bus.read(((std::uint16_t)hi << 8) | lo0);
    hh = _bus.read(((std::uint16_t)hi << 8) | lo1);

    return ((std::uint16_t)hh << 8) | ll;
}
