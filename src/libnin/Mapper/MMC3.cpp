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
#include <libnin/Mapper/MMC3.h>
#include <libnin/IRQ.h>
#include <libnin/Util.h>

using namespace libnin;

MapperMMC3::MapperMMC3(Memory& memory, Cart& cart, IRQ& irq)
: Mapper{memory, cart, irq}
, _bankSelect{}
, _bank{}
, _bankModePrgRom{}
, _bankModeChrRom{}
, _irqScanlineEnabled{}
, _irqScanlineReload{}
, _irqScanlineCounter{}
, _irqScanlineReloadValue{}
, _irqScanlineFilterShifter{}
, _oldVmemAddr{}
{

}

void MapperMMC3::handleReset()
{
    _bank[6] = 0;
    _bank[7] = 1;
    apply();
}

void MapperMMC3::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe001)
    {
    case 0x8000: // Bank select
        _bankSelect = value & 0x7;
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
                _irqScanlineReload = 0;
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
