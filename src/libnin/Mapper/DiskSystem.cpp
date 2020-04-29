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

#include <libnin/Cart.h>
#include <libnin/Disk.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

using namespace libnin;

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
        value = _diskSystem.latchRead;
        _diskSystem.transfered = 0;
        _irq.unset(IRQ_MAPPER2);
        break;
    case 0x4032: // Disk Drive Status
        value = 0x40;
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
        _diskSystem.irqReloadFlag = !!(value & 0x01);
        _diskSystem.irqEnabledTimer = !!(value & 0x02);
        _diskSystem.irqTimer = _diskSystem.irqReloadValue;
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
        break;
    case 0x4025: // FDS Control
        _diskSystem.motor = !!(value & 0x01);
        _diskSystem.noScan = !!(value & 0x02);
        _diskSystem.inData = !!(value & 0x40);
        _diskSystem.irqEnabledTransfer = !!(value & 0x80);
        mirror((value & 0x08) ? NIN_MIRROR_V : NIN_MIRROR_H);
        break;
    case 0x4026: // External
        _diskSystem.extPort = value;
        break;
    }
}

template<>
void Mapper::handleTick<MapperID::FDS>()
{
    std::uint8_t tmp;
    int skip{};

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
        _diskSystem.scanning = 0;
        _diskSystem.endOfDisk = 1;
        return;
    }
    else if (_diskSystem.noScan && !_diskSystem.scanning)
    {
        return;
    }
    else if (_diskSystem.endOfDisk)
    {
        _diskSystem.endOfDisk = 0;
        _diskSystem.delay = 10000;
        _diskSystem.headPos = 0;
        _diskSystem.skippedGap = 0;
        return;
    }
    else if (_diskSystem.delay)
    {
        _diskSystem.delay--;
        return;
    }

    _diskSystem.scanning = 1;
    tmp = _disk.read(0, _diskSystem.headPos++);

    if (!_diskSystem.inData)
    {
        _diskSystem.skippedGap = 0;
    }
    else if (tmp && !_diskSystem.skippedGap)
    {
        _diskSystem.skippedGap = 1;
        skip = 1;
    }

    if (_diskSystem.skippedGap && !skip)
    {
        _diskSystem.transfered = 1;
        _diskSystem.latchRead = tmp;
        if (_diskSystem.irqEnabledTransfer)
        {
            _irq.set(IRQ_MAPPER2);
        }
    }

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
    _handleRead = &Mapper::handleRead<MapperID::FDS>;
    _handleWrite = &Mapper::handleWrite<MapperID::FDS>;
    _handleTick = &Mapper::handleTick<MapperID::FDS>;
}
