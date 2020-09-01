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
#include <libnin/Mapper.h>
#include <libnin/Util.h>

namespace libnin
{

static void apply(Mapper& mapper, MapperMMC2& mmc2)
{
    mapper.bankChr4k(0, mmc2.latch0 ? mmc2.bankLatch0Hi : mmc2.bankLatch0Lo);
    mapper.bankChr4k(1, mmc2.latch1 ? mmc2.bankLatch1Hi : mmc2.bankLatch1Lo);
}

template <>
void Mapper::handleReset<MapperID::MMC2>()
{
    _mmc2 = MapperMMC2{};

    bankPrg8k(3, CART_PRG_ROM, -3);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);
}

template <>
void Mapper::handleWrite<MapperID::MMC2>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        bankPrg8k(2, CART_PRG_ROM, value & 0xf);
        break;
    case 0xb000:
        _mmc2.bankLatch0Lo = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xc000:
        _mmc2.bankLatch0Hi = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xd000:
        _mmc2.bankLatch1Lo = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xe000:
        _mmc2.bankLatch1Hi = value & 0x1f;
        apply(*this, _mmc2);
        break;
    case 0xf000:
        if (value & 0x1)
            mirror(NIN_MIRROR_V);
        else
            mirror(NIN_MIRROR_H);
    }
}

template <>
void Mapper::handleVideoRead<MapperID::MMC2>(std::uint16_t addr)
{
    if (addr == 0x0fd8)
    {
        _mmc2.latch0 = 0;
        apply(*this, _mmc2);
    }
    else if (addr == 0x0fe8)
    {
        _mmc2.latch0 = 1;
        apply(*this, _mmc2);
    }
    else if (addr >= 0x1fd8 && addr <= 0x1fdf)
    {
        _mmc2.latch1 = 0;
        apply(*this, _mmc2);
    }
    else if (addr >= 0x1fe8 && addr <= 0x1fef)
    {
        _mmc2.latch1 = 1;
        apply(*this, _mmc2);
    }
}

template <>
void Mapper::init<MapperID::MMC2>()
{
    _handleReset     = &Mapper::handleReset<MapperID::MMC2>;
    _handleWrite     = &Mapper::handleWrite<MapperID::MMC2>;
    _handleVideoRead = &Mapper::handleVideoRead<MapperID::MMC2>;
}

}
