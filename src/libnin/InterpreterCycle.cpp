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

#include <libnin/APU.h>
#include <libnin/BusMain.h>
#include <libnin/InterpreterCycle.h>
#include <libnin/IRQ.h>
#include <libnin/NMI.h>
#include <libnin/PPU.h>

using namespace libnin;

InterpreterCycle::InterpreterCycle(CPU& cpu, Memory& memory, IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus)
: _cpu{cpu}
, _memory{memory}
, _irq{irq}
, _nmi{nmi}
, _ppu{ppu}
, _apu{apu}
, _bus{bus}
, _handler{kOps[0x100]}
, _addr{}
, _nmiPending{}
, _irqPending{}
, _odd{}
, _reset{true}
{
}

std::size_t InterpreterCycle::tick(std::size_t cycles)
{
    Handler handler;

    handler = _handler;
    for (std::size_t i = 0; i < cycles; ++i)
    {
        handler = (this->*handler)();
        _odd    = !_odd;
    }
    _handler = handler;
    return 0;
}

InterpreterCycle::Handler InterpreterCycle::dispatch()
{
    Handler       handler;
    std::uint16_t op;

    if (_nmiPending)
    {
        _nmiPending = false;
        _irqPending = false;

        op = 0x102;
        _nmi.ack();
    }
    else if (_irqPending)
    {
        _irqPending = false;
        op          = 0x101;
    }
    else
    {
        op = read(_cpu.pc++);
    }
    handler = kOps[op];
    return (this->*handler)();
}

InterpreterCycle::Handler InterpreterCycle::kil()
{
    return &InterpreterCycle::kil;
}

InterpreterCycle::Handler InterpreterCycle::dma()
{
    if (_odd)
        return &InterpreterCycle::dma;
    return &InterpreterCycle::dmaRead;
}

InterpreterCycle::Handler InterpreterCycle::dmaRead()
{
    _dmaValue = read(_dmaAddr++);
    return &InterpreterCycle::dmaWrite;
}

InterpreterCycle::Handler InterpreterCycle::dmaWrite()
{
    _ppu.oamWrite(_dmaValue);
    _dmaCount++;
    return _dmaCount ? Handler(&InterpreterCycle::dmaRead) : _handler2;
}

std::uint8_t InterpreterCycle::read(std::uint16_t addr)
{
    return _bus.read(addr);
}

InterpreterCycle::Handler InterpreterCycle::write(std::uint16_t addr, std::uint8_t value, Handler next)
{
    switch (_bus.write(addr, value))
    {
    case WriteAction::DMA:
        _handler2 = next;
        _dmaAddr  = ((std::uint16_t)value << 8);
        _dmaCount = 0;
        return &InterpreterCycle::dma;
    default:
        break;
    }
    return next;
}

std::uint8_t InterpreterCycle::adc(std::uint8_t a, std::uint8_t b)
{
    std::uint16_t sum;
    std::uint8_t  carryIn;

    carryIn = (_cpu.p & PFLAG_C) ? 1 : 0;
    sum     = (std::uint16_t)a + (std::uint16_t)b + (std::uint16_t)carryIn;

    _cpu.p &= ~(PFLAG_C | PFLAG_V | PFLAG_N | PFLAG_Z);
    if (sum & 0x100) _cpu.p |= PFLAG_C;
    if ((sum & 0xff) == 0) _cpu.p |= PFLAG_Z;
    if ((a ^ sum) & (b ^ sum) & 0x80) _cpu.p |= PFLAG_V;
    if (sum & 0x80) _cpu.p |= PFLAG_N;

    return (sum & 0xff);
}
