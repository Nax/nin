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
#include <libnin/Util.h>

using namespace libnin;

void Mapper::mmc2Common(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xf000)
    {
    case 0xb000:
        _mmc2.bankLatch0Lo = value & 0x1f;
        mmc2Apply();
        break;
    case 0xc000:
        _mmc2.bankLatch0Hi = value & 0x1f;
        mmc2Apply();
        break;
    case 0xd000:
        _mmc2.bankLatch1Lo = value & 0x1f;
        mmc2Apply();
        break;
    case 0xe000:
        _mmc2.bankLatch1Hi = value & 0x1f;
        mmc2Apply();
        break;
    case 0xf000:
        if (value & 0x1)
            mirror(NIN_MIRROR_V);
        else
            mirror(NIN_MIRROR_H);
    }
}

void Mapper::mmc2Apply()
{
    bankChr4k(0, _mmc2.latch0 ? _mmc2.bankLatch0Hi : _mmc2.bankLatch0Lo);
    bankChr4k(1, _mmc2.latch1 ? _mmc2.bankLatch1Hi : _mmc2.bankLatch1Lo);
}

void Mapper::write_MMC2(std::uint16_t addr, std::uint8_t value)
{
    mmc2Common(addr, value);
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        bankPrg8k(0, value & 0xf);
        break;
    }
}

void Mapper::videoRead_MMC2(std::uint16_t addr)
{
    if (addr == 0x0fd8)
    {
        _mmc2.latch0 = 0;
        mmc2Apply();
    }
    else if (addr == 0x0fe8)
    {
        _mmc2.latch0 = 1;
        mmc2Apply();
    }
    else if (addr >= 0x1fd8 && addr <= 0x1fdf)
    {
        _mmc2.latch1 = 0;
        mmc2Apply();
    }
    else if (addr >= 0x1fe8 && addr <= 0x1fef)
    {
        _mmc2.latch1 = 1;
        mmc2Apply();
    }
}
