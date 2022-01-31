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

#include <libnin/APU.h>
#include <libnin/BusMain.h>
#include <libnin/CPU.h>
#include <libnin/IRQ.h>
#include <libnin/NMI.h>
#include <libnin/PPU.h>

using namespace libnin;

CPU::CPU(Memory& memory, IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus)
: _memory{memory}
, _irq{irq}
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
, _irqPending{}
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
        _odd    = !_odd;
    }
    _handler = handler;
    return 0;
}

CPU::Handler CPU::dispatch()
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
        op = read(_pc++);
    }
    handler = kOps[op];
    return (this->*handler)();
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
        _dmaAddr  = ((std::uint16_t)value << 8);
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
    std::uint8_t  carryIn;

    carryIn = (_p & PFLAG_C) ? 1 : 0;
    sum     = (std::uint16_t)a + (std::uint16_t)b + (std::uint16_t)carryIn;

    _p &= ~(PFLAG_C | PFLAG_V | PFLAG_N | PFLAG_Z);
    if (sum & 0x100) _p |= PFLAG_C;
    if ((sum & 0xff) == 0) _p |= PFLAG_Z;
    if ((a ^ sum) & (b ^ sum) & 0x80) _p |= PFLAG_V;
    if (sum & 0x80) _p |= PFLAG_N;

    return (sum & 0xff);
}
