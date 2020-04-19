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

CPU::CPU(Memory& memory, IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus)
: _memory{memory}
, _irq {irq}
, _nmi{nmi}
, _ppu{ppu}
, _apu{apu}
, _bus{bus}
, _handler{kOps[0x100]}
, _pc{}
, _addr{}
, _regs{}
, _p{PFLAG_I}
, _p2{}
, _nmiPending{}
, _odd{}
, _reset{true}
{

}

std::size_t CPU::tick(std::size_t cycles)
{
    Handler handler;

    handler = _handler;
    for (std::size_t i = 0; i < cycles; ++i)
    {
        handler = (this->*handler)();
        _odd = !_odd;
    }
    _handler = handler;
    return 0;
}

CPU::Handler CPU::dispatch()
{
    std::uint16_t op;

    if (_nmiPending)
    {
        op = 0x102;
        _nmi.ack();
    }
    else if (_irq.high() && !(_p & PFLAG_I))
    {
        op = 0x101;
    }
    else
    {
        op = read(_pc++);
    }
    _nmiPending = _nmi.high();
    return kOps[op];
}

CPU::Handler CPU::kil()
{
    return &CPU::kil;
}

CPU::Handler CPU::dma()
{
    if (_odd)
        return &CPU::dma;
    return &CPU::dmaRead;
}

CPU::Handler CPU::dmaRead()
{
    _dmaValue = read(_dmaAddr++);
    return &CPU::dmaWrite;
}

CPU::Handler CPU::dmaWrite()
{
    _ppu.oamWrite(_dmaValue);
    _dmaCount++;
    return _dmaCount ? Handler(&CPU::dmaRead) : _handler2;
}

std::uint8_t CPU::read(std::uint16_t addr)
{
    return _bus.read(addr);
}

CPU::Handler CPU::write(std::uint16_t addr, std::uint8_t value, Handler next)
{
    switch (_bus.write(addr, value))
    {
    case WriteAction::DMA:
        _handler2 = next;
        _dmaAddr = ((std::uint16_t)value << 8);
        _dmaCount = 0;
        return &CPU::dma;
    default:
        break;
    }
    return next;
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
