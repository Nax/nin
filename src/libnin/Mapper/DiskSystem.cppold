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

#include <libnin/Cart.h>
#include <libnin/Disk.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

namespace libnin
{

template <>
void Mapper::handleReset<MapperID::FDS>()
{
    _diskSystem = MapperDiskSystem{};
    bankPrg32k(1, CART_PRG_RAM, 0);
    bankPrg8k(5, CART_PRG_ROM, 0);
}

template <>
std::uint8_t Mapper::handleRead<MapperID::FDS>(std::uint16_t addr)
{
    std::uint8_t value{};

    switch (addr)
    {
    case 0x4030: // Disk Status
        value = 0x80;
        if (_irq.check(IRQ_MAPPER1)) value |= 0x01;
        if (_diskSystem.transfered)
            value |= 0x02;
        _diskSystem.transfered = 0;
        if (_diskSystem.endOfDisk)
            value |= 0x40;
        _irq.unset(IRQ_MAPPER1 | IRQ_MAPPER2);
        break;
    case 0x4031: // Disk Read
        value                  = _diskSystem.latchRead;
        _diskSystem.transfered = 0;
        _irq.unset(IRQ_MAPPER2);
        break;
    case 0x4032: // Disk Drive Status
        value = 0x40;
        if (_disk.inserted())
            value |= 0x00;
        else
            value |= 0x07;
        if (!_diskSystem.scanning)
            value |= 0x02;
        break;
    case 0x4033: // Ext Read
        value = 0x80;
        break;
    }
    return value;
}

template <>
void Mapper::handleWrite<MapperID::FDS>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x4020:
        _diskSystem.irqReloadValue &= 0xff00;
        _diskSystem.irqReloadValue |= value;
        break;
    case 0x4021:
        _diskSystem.irqReloadValue &= 0x00ff;
        _diskSystem.irqReloadValue |= ((uint16_t)value << 8);
        break;
    case 0x4022:
        _diskSystem.irqReloadFlag   = !!(value & 0x01);
        _diskSystem.irqEnabledTimer = !!(value & 0x02);
        _diskSystem.irqTimer        = _diskSystem.irqReloadValue;
        if (!_diskSystem.irqEnabledTimer)
            _irq.unset(IRQ_MAPPER1);
        break;
    case 0x4023:
        if (!(value & 0x01))
            _irq.unset(IRQ_MAPPER1);
        break;
    case 0x4024: // Disk write
        _irq.unset(IRQ_MAPPER2);
        _diskSystem.transfered = 0;
        _diskSystem.latchWrite = value;
        break;
    case 0x4025: // FDS Control
        _diskSystem.motor              = !!(value & 0x01);
        _diskSystem.noScan             = !!(value & 0x02);
        _diskSystem.write              = !(value & 0x04);
        _diskSystem.transmitCRC        = !!(value & 0x10);
        _diskSystem.inData             = !!(value & 0x40);
        _diskSystem.irqEnabledTransfer = !!(value & 0x80);
        mirror((value & 0x08) ? NIN_MIRROR_V : NIN_MIRROR_H);
        break;
    case 0x4026: // External
        _diskSystem.extPort = value;
        break;
    }
}

template <>
void Mapper::handleTick<MapperID::FDS>()
{
    std::uint8_t tmp;
    int          skip{};

    _disk.tick();
    if (_diskSystem.irqEnabledTimer)
    {
        if (_diskSystem.irqTimer)
            _diskSystem.irqTimer--;
        else
        {
            _irq.set(IRQ_MAPPER1);
            if (_diskSystem.irqReloadFlag)
                _diskSystem.irqTimer = _diskSystem.irqReloadValue;
            else
                _diskSystem.irqEnabledTimer = 0;
        }
    }

    if (!_diskSystem.motor)
    {
        _diskSystem.scanning  = 0;
        _diskSystem.endOfDisk = 1;
        return;
    }
    else if (_diskSystem.noScan && !_diskSystem.scanning)
    {
        return;
    }
    else if (_diskSystem.endOfDisk)
    {
        _diskSystem.endOfDisk  = 0;
        _diskSystem.delay      = 10000;
        _diskSystem.headPos    = 0;
        _diskSystem.skippedGap = 0;
        return;
    }
    else if (_diskSystem.delay)
    {
        _diskSystem.delay--;
        return;
    }

    _diskSystem.scanning = 1;

    if (!_diskSystem.write)
    {
        /* Read */
        tmp = _disk.read(_diskSystem.headPos);

        if (!_diskSystem.inData)
        {
            _diskSystem.skippedGap = 0;
        }
        else if (tmp && !_diskSystem.skippedGap)
        {
            _diskSystem.skippedGap = 1;
            skip                   = 1;
        }

        if (_diskSystem.skippedGap && !skip)
        {
            _diskSystem.transfered = 1;
            _diskSystem.latchRead  = tmp;
            if (_diskSystem.irqEnabledTransfer)
            {
                _irq.set(IRQ_MAPPER2);
            }
        }
    }
    else
    {
        /* Write */
        if (!_diskSystem.transmitCRC)
        {
            _diskSystem.transfered = 1;
            if (_diskSystem.irqEnabledTransfer)
            {
                _irq.set(IRQ_MAPPER2);
            }
        }
        if (!_diskSystem.inData)
            tmp = 0x00;
        else
            tmp = _diskSystem.latchWrite;
        _disk.write(_diskSystem.headPos, tmp);
        _diskSystem.skippedGap = 0;
    }

    _diskSystem.headPos++;
    if (_diskSystem.headPos >= Disk::DiskSize)
    {
        _diskSystem.motor = 0;
    }
    else
    {
        _diskSystem.delay = 149;
    }
}

template <>
void Mapper::init<MapperID::FDS>()
{
    _handleReset = &Mapper::handleReset<MapperID::FDS>;
    _handleRead  = &Mapper::handleRead<MapperID::FDS>;
    _handleWrite = &Mapper::handleWrite<MapperID::FDS>;
    _handleTick  = &Mapper::handleTick<MapperID::FDS>;
}

}
