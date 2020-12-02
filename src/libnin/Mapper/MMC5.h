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
#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC5 : public Mapper
{
public:
    void         handleInit();
    void         handleTick();
    std::uint8_t handleRead(std::uint16_t addr);
    void         handleWrite(std::uint16_t addr, std::uint8_t value);
    void         handleVideoRead(std::uint16_t addr);
    std::uint8_t handleNtRead(int nametable, std::uint16_t off);
    void         handleNtWrite(int nametable, std::uint16_t off, std::uint8_t value);
    std::uint8_t handleChrRead(int bank, std::uint16_t offset);

private:
    void applyPrg();
    void applyChr();

    std::uint8_t  _bankModePrg : 2;
    std::uint8_t  _bankModeChr : 2;
    std::uint8_t  _bankSelectPrg[5];
    std::uint8_t  _bankSelectChr[12];
    std::uint8_t  _nametable;
    std::uint8_t  _fillNT;
    std::uint8_t  _fillAT;
    std::uint8_t  _mul[2];
    std::uint8_t* _chr2[8];

    std::uint16_t _ppuAddr;
    std::uint8_t  _ppuAddrCount;
    std::uint8_t  _ppuIdleCount;
    std::uint8_t  _fetchCount;
    std::uint8_t  _scanline;
    std::uint8_t  _scanlineTarget;

    bool _ppuReading : 1;
    bool _ppuSpriteFlag : 1;
    bool _ppuRenderFlag : 1;
    bool _mode8x16 : 1;
    bool _inFrame : 1;
    bool _scanlineEnabled : 1;
    bool _scanlinePending : 1;
};

} // namespace libnin

#endif
