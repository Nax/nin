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

#ifndef LIBNIN_MAPPER_MMC5_H
#define LIBNIN_MAPPER_MMC5_H 1

#include <cstdint>

namespace libnin
{

struct MapperMMC5
{
    std::uint8_t  bankModePrg : 2;
    std::uint8_t  bankModeChr : 2;
    std::uint8_t  bankSelectPrg[5];
    std::uint8_t  bankSelectChr[12];
    std::uint8_t  nametable;
    std::uint8_t  fillNT;
    std::uint8_t  fillAT;
    std::uint8_t  mul[2];
    std::uint8_t* chr2[8];

    std::uint16_t ppuAddr;
    std::uint8_t  ppuAddrCount;
    std::uint8_t  ppuIdleCount;
    std::uint8_t  fetchCount;
    std::uint8_t  scanline;
    std::uint8_t  scanlineTarget;

    bool ppuReading : 1;
    bool ppuSpriteFlag : 1;
    bool ppuRenderFlag : 1;
    bool mode8x16 : 1;
    bool inFrame : 1;
    bool scanlineEnabled : 1;
    bool scanlinePending : 1;
};

} // namespace libnin

#endif
