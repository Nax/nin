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
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

namespace libnin
{

static void apply(Mapper& mapper, MapperMMC3& mmc3)
{
    if (mmc3.bankModePrgRom == 0)
    {
        mapper.bankPrg8k(2, CART_PRG_ROM, mmc3.bank[6]);
        mapper.bankPrg8k(3, CART_PRG_ROM, mmc3.bank[7]);
        mapper.bankPrg8k(4, CART_PRG_ROM, -2);
        mapper.bankPrg8k(5, CART_PRG_ROM, -1);
    }
    else
    {
        mapper.bankPrg8k(2, CART_PRG_ROM, -2);
        mapper.bankPrg8k(3, CART_PRG_ROM, mmc3.bank[7]);
        mapper.bankPrg8k(4, CART_PRG_ROM, mmc3.bank[6]);
        mapper.bankPrg8k(5, CART_PRG_ROM, -1);
    }

    if (mmc3.bankModeChrRom == 0)
    {
        mapper.bankChr1k(0, mmc3.bank[0] & ~1);
        mapper.bankChr1k(1, mmc3.bank[0] | 1);
        mapper.bankChr1k(2, mmc3.bank[1] & ~1);
        mapper.bankChr1k(3, mmc3.bank[1] | 1);
        mapper.bankChr1k(4, mmc3.bank[2]);
        mapper.bankChr1k(5, mmc3.bank[3]);
        mapper.bankChr1k(6, mmc3.bank[4]);
        mapper.bankChr1k(7, mmc3.bank[5]);
    }
    else
    {
        mapper.bankChr1k(0, mmc3.bank[2]);
        mapper.bankChr1k(1, mmc3.bank[3]);
        mapper.bankChr1k(2, mmc3.bank[4]);
        mapper.bankChr1k(3, mmc3.bank[5]);
        mapper.bankChr1k(4, mmc3.bank[0] & ~1);
        mapper.bankChr1k(5, mmc3.bank[0] | 1);
        mapper.bankChr1k(6, mmc3.bank[1] & ~1);
        mapper.bankChr1k(7, mmc3.bank[1] | 1);
    }
}

template <>
void Mapper::handleReset<MapperID::MMC3>()
{
    _mmc3 = MapperMMC3{};

    _mmc3.bank[6] = 0;
    _mmc3.bank[7] = 1;
    apply(*this, _mmc3);
}

template <>
void Mapper::handleWrite<MapperID::MMC3>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe001)
    {
    case 0x8000: // Bank select
        _mmc3.bankSelect     = value & 0x7;
        _mmc3.bankModePrgRom = (value >> 6) & 0x01;
        _mmc3.bankModeChrRom = (value >> 7) & 0x01;
        apply(*this, _mmc3);
        break;
    case 0x8001: // Bank data
        _mmc3.bank[_mmc3.bankSelect] = value;
        apply(*this, _mmc3);
        break;
    case 0xa000:
        if ((value & 0x01) == 0)
            mirror(NIN_MIRROR_H);
        else
            mirror(NIN_MIRROR_V);
        break;
    case 0xc000:
        _mmc3.irqScanlineReloadValue = value;
        break;
    case 0xc001:
        _mmc3.irqScanlineReload = 1;
        break;
    case 0xe000:
        _mmc3.irqScanlineEnabled = 0;
        _irq.unset(IRQ_MAPPER1);
        break;
    case 0xe001:
        _mmc3.irqScanlineEnabled = 1;
        break;
    }
}

template <>
void Mapper::handleVideoRead<MapperID::MMC3>(std::uint16_t addr)
{
    if (addr >= 0x3f00)
        return;

    if (((_mmc3.oldVmemAddr & 0x1000) == 0x0000) && ((addr & 0x1000) == 0x1000))
    {
        if (!_mmc3.irqScanlineFilterShifter)
        {
            if (_mmc3.irqScanlineCounter == 0 && _mmc3.irqScanlineEnabled)
            {
                _irq.set(IRQ_MAPPER1);
            }
            if (_mmc3.irqScanlineCounter == 0 || _mmc3.irqScanlineReload)
            {
                _mmc3.irqScanlineCounter = _mmc3.irqScanlineReloadValue;
                _mmc3.irqScanlineReload  = 0;
            }
            else
                _mmc3.irqScanlineCounter--;
        }
        _mmc3.irqScanlineFilterShifter = 0x8000;
    }
    else
        _mmc3.irqScanlineFilterShifter >>= 1;
    _mmc3.oldVmemAddr = addr;
}

template <>
void Mapper::init<MapperID::MMC3>()
{
    _handleReset     = &Mapper::handleReset<MapperID::MMC3>;
    _handleWrite     = &Mapper::handleWrite<MapperID::MMC3>;
    _handleVideoRead = &Mapper::handleVideoRead<MapperID::MMC3>;
}

}
