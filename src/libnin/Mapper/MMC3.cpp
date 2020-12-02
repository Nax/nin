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
#include <libnin/Mapper/MMC3.h>
#include <libnin/Util.h>

namespace libnin
{

void MapperMMC3::handleInit()
{
    _bankSelect               = 0;
    _bank[0]                  = 0;
    _bank[1]                  = 0;
    _bank[2]                  = 0;
    _bank[3]                  = 0;
    _bank[4]                  = 0;
    _bank[5]                  = 0;
    _bank[6]                  = 0;
    _bank[7]                  = 1;
    _bankModePrgRom           = 0;
    _bankModeChrRom           = 0;
    _irqScanlineEnabled       = 0;
    _irqScanlineReload        = 0;
    _irqScanlineCounter       = 0;
    _irqScanlineReloadValue   = 0;
    _irqScanlineFilterShifter = 0;
    _oldVmemAddr              = 0;

    apply();
}

void MapperMMC3::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe001)
    {
    case 0x8000: // Bank select
        _bankSelect     = value & 0x7;
        _bankModePrgRom = (value >> 6) & 0x01;
        _bankModeChrRom = (value >> 7) & 0x01;
        apply();
        break;
    case 0x8001: // Bank data
        _bank[_bankSelect] = value;
        apply();
        break;
    case 0xa000:
        if ((value & 0x01) == 0)
            mirror(NIN_MIRROR_H);
        else
            mirror(NIN_MIRROR_V);
        break;
    case 0xc000:
        _irqScanlineReloadValue = value;
        break;
    case 0xc001:
        _irqScanlineReload = 1;
        break;
    case 0xe000:
        _irqScanlineEnabled = 0;
        _irq.unset(IRQ_MAPPER1);
        break;
    case 0xe001:
        _irqScanlineEnabled = 1;
        break;
    }
}

void MapperMMC3::handleVideoRead(std::uint16_t addr)
{
    if (addr >= 0x3f00)
        return;

    if (((_oldVmemAddr & 0x1000) == 0x0000) && ((addr & 0x1000) == 0x1000))
    {
        if (!_irqScanlineFilterShifter)
        {
            if (_irqScanlineCounter == 0 && _irqScanlineEnabled)
            {
                _irq.set(IRQ_MAPPER1);
            }
            if (_irqScanlineCounter == 0 || _irqScanlineReload)
            {
                _irqScanlineCounter = _irqScanlineReloadValue;
                _irqScanlineReload  = 0;
            }
            else
                _irqScanlineCounter--;
        }
        _irqScanlineFilterShifter = 0x8000;
    }
    else
        _irqScanlineFilterShifter >>= 1;
    _oldVmemAddr = addr;
}

void MapperMMC3::apply()
{
    if (_bankModePrgRom == 0)
    {
        bankPrg8k(2, CART_PRG_ROM, _bank[6]);
        bankPrg8k(3, CART_PRG_ROM, _bank[7]);
        bankPrg8k(4, CART_PRG_ROM, -2);
        bankPrg8k(5, CART_PRG_ROM, -1);
    }
    else
    {
        bankPrg8k(2, CART_PRG_ROM, -2);
        bankPrg8k(3, CART_PRG_ROM, _bank[7]);
        bankPrg8k(4, CART_PRG_ROM, _bank[6]);
        bankPrg8k(5, CART_PRG_ROM, -1);
    }

    if (_bankModeChrRom == 0)
    {
        bankChr1k(0, _bank[0] & ~1);
        bankChr1k(1, _bank[0] | 1);
        bankChr1k(2, _bank[1] & ~1);
        bankChr1k(3, _bank[1] | 1);
        bankChr1k(4, _bank[2]);
        bankChr1k(5, _bank[3]);
        bankChr1k(6, _bank[4]);
        bankChr1k(7, _bank[5]);
    }
    else
    {
        bankChr1k(0, _bank[2]);
        bankChr1k(1, _bank[3]);
        bankChr1k(2, _bank[4]);
        bankChr1k(3, _bank[5]);
        bankChr1k(4, _bank[0] & ~1);
        bankChr1k(5, _bank[0] | 1);
        bankChr1k(6, _bank[1] & ~1);
        bankChr1k(7, _bank[1] | 1);
    }
}

} // namespace libnin
