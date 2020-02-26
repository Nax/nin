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

#include <libnin/Mapper.h>
#include <libnin/IRQ.h>
#include <libnin/Util.h>

using namespace libnin;

void Mapper::mmc3Apply()
{
    if (_mmc3.bankModePrgRom == 0)
    {
        bankPrg8k(0, _mmc3.bank[6]);
        bankPrg8k(1, _mmc3.bank[7]);
        bankPrg8k(2, -2);
        bankPrg8k(3, -1);
    }
    else
    {
        bankPrg8k(0, -2);
        bankPrg8k(1, _mmc3.bank[7]);
        bankPrg8k(2, _mmc3.bank[6]);
        bankPrg8k(3, -1);
    }

    if (_mmc3.bankModeChrRom == 0)
    {
        bankChr1k(0, _mmc3.bank[0] & ~1);
        bankChr1k(1, _mmc3.bank[0] | 1);
        bankChr1k(2, _mmc3.bank[1] & ~1);
        bankChr1k(3, _mmc3.bank[1] | 1);
        bankChr1k(4, _mmc3.bank[2]);
        bankChr1k(5, _mmc3.bank[3]);
        bankChr1k(6, _mmc3.bank[4]);
        bankChr1k(7, _mmc3.bank[5]);
    }
    else
    {
        bankChr1k(0, _mmc3.bank[2]);
        bankChr1k(1, _mmc3.bank[3]);
        bankChr1k(2, _mmc3.bank[4]);
        bankChr1k(3, _mmc3.bank[5]);
        bankChr1k(4, _mmc3.bank[0] & ~1);
        bankChr1k(5, _mmc3.bank[0] | 1);
        bankChr1k(6, _mmc3.bank[1] & ~1);
        bankChr1k(7, _mmc3.bank[1] | 1);
    }
}

void Mapper::write_MMC3(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe001)
    {
    case 0x8000: // Bank select
        _mmc3.bankSelect = value & 0x7;
        _mmc3.bankModePrgRom = (value >> 6) & 0x01;
        _mmc3.bankModeChrRom = (value >> 7) & 0x01;
        mmc3Apply();
        break;
    case 0x8001: // Bank data
        _mmc3.bank[_mmc3.bankSelect] = value;
        mmc3Apply();
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
        _irq.unset(IRQ_SCANLINE);
        break;
    case 0xe001:
        _mmc3.irqScanlineEnabled = 1;
        break;
    }
}

void Mapper::videoRead_MMC3(std::uint16_t addr)
{
    if (addr >= 0x3f00)
        return;

    if (((_mmc3.oldVmemAddr & 0x1000) == 0x0000) && ((addr & 0x1000) == 0x1000))
    {
        if (!_mmc3.irqScanlineFilterShifter)
        {
            if (_mmc3.irqScanlineCounter == 0 && _mmc3.irqScanlineEnabled)
            {
                _irq.set(IRQ_SCANLINE);
            }
            if (_mmc3.irqScanlineCounter == 0 || _mmc3.irqScanlineReload)
            {
                _mmc3.irqScanlineCounter = _mmc3.irqScanlineReloadValue;
                _mmc3.irqScanlineReload = 0;
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
