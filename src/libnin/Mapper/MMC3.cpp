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
    _prgBankAnd               = 0xffffffff;
    _prgBankOr                = 0;
    _chrBankAnd               = 0xffffffff;
    _chrBankOr                = 0;

    apply();
}

void MapperMMC3::handleInit_Multi37()
{
    handleInit();

    _prgBankAnd = 0x07;
    _prgBankOr  = 0x00;
    _chrBankAnd = 0x7f;
    _chrBankOr  = 0x00;

    apply();
}

void MapperMMC3::handleInit_Multi47()
{
    handleInit();

    _prgBankAnd = 0x0f;
    _prgBankOr  = 0x00;
    _chrBankAnd = 0x7f;
    _chrBankOr  = 0x00;

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
        if (_mirrorMode != NIN_MIRROR_4)
        {
            if ((value & 0x01) == 0)
                mirror(NIN_MIRROR_H);
            else
                mirror(NIN_MIRROR_V);
        }
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

void MapperMMC3::handleWrite_Multi37(std::uint16_t addr, std::uint8_t value)
{
    if ((addr & 0xe000) == 0x6000)
    {
        switch (value & 7)
        {
        case 0:
        case 1:
        case 2:
            _prgBankAnd = 0x07;
            _prgBankOr  = 0x00;
            _chrBankAnd = 0x7f;
            _chrBankOr  = 0x00;
            break;
        case 3:
            _prgBankAnd = 0x07;
            _prgBankOr  = 0x08;
            _chrBankAnd = 0x7f;
            _chrBankOr  = 0x00;
            break;
        case 4:
        case 5:
        case 6:
            _prgBankAnd = 0x0f;
            _prgBankOr  = 0x10;
            _chrBankAnd = 0x7f;
            _chrBankOr  = 0x80;
            break;
        case 7:
            _prgBankAnd = 0x07;
            _prgBankOr  = 0x18;
            _chrBankAnd = 0x7f;
            _chrBankOr  = 0x00;
            break;
        }

        apply();
    }

    handleWrite(addr, value);
}

void MapperMMC3::handleWrite_Multi47(std::uint16_t addr, std::uint8_t value)
{
    if ((addr & 0xe000) == 0x6000)
    {
        switch (value & 1)
        {
        case 0:
            _prgBankAnd = 0x0f;
            _prgBankOr  = 0x00;
            _chrBankAnd = 0x7f;
            _chrBankOr  = 0x00;
            break;
        case 1:
            _prgBankAnd = 0x0f;
            _prgBankOr  = 0x10;
            _chrBankAnd = 0x7f;
            _chrBankOr  = 0x80;
            break;
        }

        apply();
    }

    handleWrite(addr, value);
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
        bankPrg(2, _bank[6]);
        bankPrg(3, _bank[7]);
        bankPrg(4, -2);
        bankPrg(5, -1);
    }
    else
    {
        bankPrg(2, -2);
        bankPrg(3, _bank[7]);
        bankPrg(4, _bank[6]);
        bankPrg(5, -1);
    }

    if (_bankModeChrRom == 0)
    {
        bankChr(0, _bank[0] & ~1);
        bankChr(1, _bank[0] | 1);
        bankChr(2, _bank[1] & ~1);
        bankChr(3, _bank[1] | 1);
        bankChr(4, _bank[2]);
        bankChr(5, _bank[3]);
        bankChr(6, _bank[4]);
        bankChr(7, _bank[5]);
    }
    else
    {
        bankChr(0, _bank[2]);
        bankChr(1, _bank[3]);
        bankChr(2, _bank[4]);
        bankChr(3, _bank[5]);
        bankChr(4, _bank[0] & ~1);
        bankChr(5, _bank[0] | 1);
        bankChr(6, _bank[1] & ~1);
        bankChr(7, _bank[1] | 1);
    }
}

void MapperMMC3::bankPrg(int slot, int index)
{
    bankPrg8k(slot, CART_PRG_ROM, (index & _prgBankAnd) | _prgBankOr);
}

void MapperMMC3::bankChr(int slot, int index)
{
    bankChr1k(slot, (index & _chrBankAnd) | _chrBankOr);
}

} // namespace libnin
