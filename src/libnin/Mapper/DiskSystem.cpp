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
#include <libnin/Mapper/DiskSystem.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperDiskSystem::handleInit()
{
    _headPos            = 0;
    _delay              = 0;
    _irqReloadValue     = 0;
    _irqTimer           = 0;
    _extPort            = 0;
    _latchRead          = 0;
    _latchWrite         = 0;
    _sideCount          = 0;
    _motor              = false;
    _noScan             = false;
    _inData             = false;
    _write              = false;
    _irqEnabledTransfer = false;
    _irqEnabledTimer    = false;
    _irqReloadFlag      = false;
    _transfered         = false;
    _scanning           = false;
    _skippedGap         = false;
    _endOfDisk          = false;
    _transmitCRC        = false;

    bankPrg32k(1, CART_PRG_RAM, 0);
    bankPrg8k(5, CART_PRG_ROM, 0);
}

void MapperDiskSystem::handleTick()
{
    std::uint8_t tmp;
    int          skip{};

    _disk.tick();
    if (_irqEnabledTimer)
    {
        if (_irqTimer)
            _irqTimer--;
        else
        {
            _irq.set(IRQ_MAPPER1);
            if (_irqReloadFlag)
                _irqTimer = _irqReloadValue;
            else
                _irqEnabledTimer = 0;
        }
    }

    if (!_motor)
    {
        _scanning  = 0;
        _endOfDisk = 1;
        return;
    }
    else if (_noScan && !_scanning)
    {
        return;
    }
    else if (_endOfDisk)
    {
        _endOfDisk  = 0;
        _delay      = 10000;
        _headPos    = 0;
        _skippedGap = 0;
        return;
    }
    else if (_delay)
    {
        _delay--;
        return;
    }

    _scanning = 1;

    if (!_write)
    {
        /* Read */
        tmp = _disk.read(_headPos);

        if (!_inData)
        {
            _skippedGap = 0;
        }
        else if (tmp && !_skippedGap)
        {
            _skippedGap = 1;
            skip        = 1;
        }

        if (_skippedGap && !skip)
        {
            _transfered = 1;
            _latchRead  = tmp;
            if (_irqEnabledTransfer)
            {
                _irq.set(IRQ_MAPPER2);
            }
        }
    }
    else
    {
        /* Write */
        if (!_transmitCRC)
        {
            _transfered = 1;
            if (_irqEnabledTransfer)
            {
                _irq.set(IRQ_MAPPER2);
            }
        }
        if (!_inData)
            tmp = 0x00;
        else
            tmp = _latchWrite;
        _disk.write(_headPos, tmp);
        _skippedGap = 0;
    }

    _headPos++;
    if (_headPos >= Disk::DiskSize)
    {
        _motor = 0;
    }
    else
    {
        _delay = 149;
    }
}

std::uint8_t MapperDiskSystem::handleRead(std::uint16_t addr)
{
    std::uint8_t value{};

    switch (addr)
    {
    case 0x4030: // Disk Status
        value = 0x80;
        if (_irq.check(IRQ_MAPPER1)) value |= 0x01;
        if (_transfered)
            value |= 0x02;
        _transfered = 0;
        if (_endOfDisk)
            value |= 0x40;
        _irq.unset(IRQ_MAPPER1 | IRQ_MAPPER2);
        break;
    case 0x4031: // Disk Read
        value       = _latchRead;
        _transfered = 0;
        _irq.unset(IRQ_MAPPER2);
        break;
    case 0x4032: // Disk Drive Status
        value = 0x40;
        if (_disk.inserted())
            value |= 0x00;
        else
            value |= 0x07;
        if (!_scanning)
            value |= 0x02;
        break;
    case 0x4033: // Ext Read
        value = 0x80;
        break;
    }
    return value;
}

void MapperDiskSystem::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x4020:
        _irqReloadValue &= 0xff00;
        _irqReloadValue |= value;
        break;
    case 0x4021:
        _irqReloadValue &= 0x00ff;
        _irqReloadValue |= ((uint16_t)value << 8);
        break;
    case 0x4022:
        _irqReloadFlag   = !!(value & 0x01);
        _irqEnabledTimer = !!(value & 0x02);
        _irqTimer        = _irqReloadValue;
        if (!_irqEnabledTimer)
            _irq.unset(IRQ_MAPPER1);
        break;
    case 0x4023:
        if (!(value & 0x01))
            _irq.unset(IRQ_MAPPER1);
        break;
    case 0x4024: // Disk write
        _irq.unset(IRQ_MAPPER2);
        _transfered = 0;
        _latchWrite = value;
        break;
    case 0x4025: // FDS Control
        _motor              = !!(value & 0x01);
        _noScan             = !!(value & 0x02);
        _write              = !(value & 0x04);
        _transmitCRC        = !!(value & 0x10);
        _inData             = !!(value & 0x40);
        _irqEnabledTransfer = !!(value & 0x80);
        mirror((value & 0x08) ? NIN_MIRROR_V : NIN_MIRROR_H);
        break;
    case 0x4026: // External
        _extPort = value;
        break;
    }
}

} // namespace libnin
