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
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

using namespace libnin;

void Mapper::tick_MMC5(void)
{
    if (_mmc5.ppuReading)
        _mmc5.ppuIdleCount = 0;
    else
    {
        _mmc5.ppuIdleCount++;
        if (_mmc5.ppuIdleCount == 100)
        {
            _mmc5.inFrame = false;
        }
    }
    _mmc5.ppuReading = false;
}

std::uint8_t Mapper::read_MMC5(std::uint16_t addr)
{
    std::uint8_t value{};

    switch (addr)
    {
    case 0x5204: // IRQ Control
        if (_mmc5.scanlinePending) value |= 0x80;
        if (_mmc5.inFrame) value |= 0x40;
        _mmc5.scanlinePending = false;
        _irq.unset(IRQ_MAPPER1);
        break;
    case 0x5205: // Mul Lo
        value = (_mmc5.mul[0] * _mmc5.mul[1]) & 0xff;
        break;
    case 0x5206: // Mul Hi
        value = (_mmc5.mul[0] * _mmc5.mul[1]) >> 8;
        break;
    case 0xfffa:
    case 0xfffb:
        _mmc5.inFrame = false;
        _mmc5.scanline = 0;
        _mmc5.scanlinePending = false;
        _irq.unset(IRQ_MAPPER1);
        break;
    }
    return value;
}

#include <cstdio>

void Mapper::write_MMC5(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x2000: // PPUCTRL
        _mmc5.ppuSpriteFlag = !!(value & 0x20);
        _mmc5.mode8x16 = (_mmc5.ppuRenderFlag && _mmc5.ppuSpriteFlag);
        mmc5ApplyCHR();
        break;
    case 0x2001: // PPUMASK
        _mmc5.ppuRenderFlag = !!(value & 0x18);
        _mmc5.mode8x16 = (_mmc5.ppuRenderFlag && _mmc5.ppuSpriteFlag);
        mmc5ApplyCHR();
        break;
    case 0x5100: // PRG Mode
        _mmc5.bankModePrg = value & 0x03;
        mmc5ApplyPRG();
        break;
    case 0x5101: // CHR Mode
        _mmc5.bankModeChr = value & 0x03;
        mmc5ApplyCHR();
        break;
    case 0x5113: // PRG Select
    case 0x5114:
    case 0x5115:
    case 0x5116:
    case 0x5117:
        _mmc5.bankSelectPrg[addr - 0x5113] = value;
        mmc5ApplyPRG();
        break;
    case 0x5120: // CHR Select
    case 0x5121:
    case 0x5122:
    case 0x5123:
    case 0x5124:
    case 0x5125:
    case 0x5126:
    case 0x5127:
    case 0x5128:
    case 0x5129:
    case 0x512a:
    case 0x512b:
        _mmc5.bankSelectChr[addr - 0x5120] = value;
        mmc5ApplyCHR();
        break;
    case 0x5203: // IRQ Target
        _mmc5.scanlineTarget = value;
        break;
    case 0x5204: // IRQ Control
        _mmc5.scanlineEnabled = !!(value & 0x80);
        if (_mmc5.scanlineEnabled && _mmc5.scanlinePending)
            _irq.set(IRQ_MAPPER1);
        break;
    case 0x5205: // Mul 1
        _mmc5.mul[0] = value;
        break;
    case 0x5206: // Mul 2
        _mmc5.mul[1] = value;
        break;
    }
}

void Mapper::mmc5ApplyPRG()
{
    bankPrg8k(1, CART_PRG_RAM, _mmc5.bankSelectPrg[0]);
    switch (_mmc5.bankModePrg)
    {
    case 0:
        bankPrg8k(2, CART_PRG_ROM, (_mmc5.bankSelectPrg[4] & 0x7c) | 0x00);
        bankPrg8k(3, CART_PRG_ROM, (_mmc5.bankSelectPrg[4] & 0x7c) | 0x01);
        bankPrg8k(4, CART_PRG_ROM, (_mmc5.bankSelectPrg[4] & 0x7c) | 0x02);
        bankPrg8k(5, CART_PRG_ROM, (_mmc5.bankSelectPrg[4] & 0x7c) | 0x03);
        break;
    case 1:
        bankPrg8k(2, (_mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_mmc5.bankSelectPrg[2] & 0x7e) | 0x00);
        bankPrg8k(3, (_mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_mmc5.bankSelectPrg[2] & 0x7e) | 0x01);
        bankPrg8k(4, CART_PRG_ROM, (_mmc5.bankSelectPrg[4] & 0x7e) | 0x00);
        bankPrg8k(5, CART_PRG_ROM, (_mmc5.bankSelectPrg[4] & 0x7e) | 0x01);
        break;
    case 2:
        bankPrg8k(2, (_mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_mmc5.bankSelectPrg[2] & 0x7e) | 0x00);
        bankPrg8k(3, (_mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, (_mmc5.bankSelectPrg[2] & 0x7e) | 0x01);
        bankPrg8k(4, (_mmc5.bankSelectPrg[3] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _mmc5.bankSelectPrg[3] & 0x7f);
        bankPrg8k(5, CART_PRG_ROM, _mmc5.bankSelectPrg[4] & 0x7f);
        break;
    case 3:
        bankPrg8k(2, (_mmc5.bankSelectPrg[1] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _mmc5.bankSelectPrg[1] & 0x7f);
        bankPrg8k(3, (_mmc5.bankSelectPrg[2] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _mmc5.bankSelectPrg[2] & 0x7f);
        bankPrg8k(4, (_mmc5.bankSelectPrg[3] & 0x80) ? CART_PRG_ROM : CART_PRG_RAM, _mmc5.bankSelectPrg[3] & 0x7f);
        bankPrg8k(5, CART_PRG_ROM, _mmc5.bankSelectPrg[4] & 0x7f);
        break;
    }
}

void Mapper::mmc5ApplyCHR()
{
    if (!_mmc5.mode8x16)
    {
        switch (_mmc5.bankModeChr)
        {
        case 0:
            bankChr8k(_mmc5.bankSelectChr[7]);
            break;
        case 1:
            bankChr4k(0, _mmc5.bankSelectChr[3]);
            bankChr4k(1, _mmc5.bankSelectChr[7]);
            break;
        case 2:
            bankChr2k(0, _mmc5.bankSelectChr[1]);
            bankChr2k(1, _mmc5.bankSelectChr[3]);
            bankChr2k(2, _mmc5.bankSelectChr[5]);
            bankChr2k(3, _mmc5.bankSelectChr[7]);
            break;
        case 3:
            bankChr1k(0, _mmc5.bankSelectChr[0]);
            bankChr1k(1, _mmc5.bankSelectChr[1]);
            bankChr1k(2, _mmc5.bankSelectChr[2]);
            bankChr1k(3, _mmc5.bankSelectChr[3]);
            bankChr1k(4, _mmc5.bankSelectChr[4]);
            bankChr1k(5, _mmc5.bankSelectChr[5]);
            bankChr1k(6, _mmc5.bankSelectChr[6]);
            bankChr1k(7, _mmc5.bankSelectChr[7]);
            break;
        }
    }
    else
    {
        switch (_mmc5.bankModeChr)
        {
        case 0:
            bankChr8k(_mmc5.bankSelectChr[11]);
            break;
        case 1:
            bankChr4k(0, _mmc5.bankSelectChr[11]);
            bankChr4k(1, _mmc5.bankSelectChr[11]);
            break;
        case 2:
            bankChr2k(0, _mmc5.bankSelectChr[9]);
            bankChr2k(1, _mmc5.bankSelectChr[11]);
            bankChr2k(2, _mmc5.bankSelectChr[9]);
            bankChr2k(3, _mmc5.bankSelectChr[11]);
            break;
        case 3:
            bankChr1k(0, _mmc5.bankSelectChr[8]);
            bankChr1k(1, _mmc5.bankSelectChr[9]);
            bankChr1k(2, _mmc5.bankSelectChr[10]);
            bankChr1k(3, _mmc5.bankSelectChr[11]);
            bankChr1k(4, _mmc5.bankSelectChr[8]);
            bankChr1k(5, _mmc5.bankSelectChr[9]);
            bankChr1k(6, _mmc5.bankSelectChr[10]);
            bankChr1k(7, _mmc5.bankSelectChr[11]);
            break;
        }
    }
}

void Mapper::videoRead_MMC5(std::uint16_t addr)
{
    _mmc5.ppuReading = true;

    if (_mmc5.ppuAddrCount == 0 && addr >= 0x2000 && addr < 0x3000)
    {
        _mmc5.ppuAddr = addr;
        _mmc5.ppuAddrCount = 1;
    }
    else
    {
        if (addr != _mmc5.ppuAddr)
            _mmc5.ppuAddrCount = 0;
        else if (_mmc5.ppuAddrCount < 3)
        {
            _mmc5.ppuAddrCount++;
            if (_mmc5.ppuAddrCount == 3)
            {
                if (!_mmc5.inFrame)
                {
                    _mmc5.inFrame = true;
                    _mmc5.scanline = 0;
                    _irq.unset(IRQ_MAPPER1);
                    printf("SCANLINE: %d, TARGET: %d\n", _mmc5.scanline, _mmc5.scanlineTarget);
                }
                else
                {
                    _mmc5.scanline++;
                    printf("SCANLINE: %d, TARGET: %d\n", _mmc5.scanline, _mmc5.scanlineTarget);
                    if (_mmc5.scanline == 241)
                    {
                        _mmc5.inFrame = false;
                        _mmc5.scanline = 0;
                        _mmc5.scanlinePending = false;
                        _irq.unset(IRQ_MAPPER1);
                    }
                    else if (_mmc5.scanline == _mmc5.scanlineTarget)
                    {
                        _mmc5.scanlinePending = true;
                        if (_mmc5.scanlineEnabled)
                            _irq.set(IRQ_MAPPER1);
                    }
                }
            }
        }
    }
}
