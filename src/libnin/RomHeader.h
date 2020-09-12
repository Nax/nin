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

#ifndef LIBNIN_ROM_HEADER_H
#define LIBNIN_ROM_HEADER_H 1

#include <cstdint>

namespace libnin
{

struct RomHeaderLegacy
{
    std::uint8_t reserved[8];
};

struct RomHeaderNes2
{
    std::uint8_t mapperEx : 4;
    std::uint8_t submapper : 4;
    std::uint8_t prgRomSizeHi : 4;
    std::uint8_t chrRomSizeHi : 4;
    std::uint8_t prgRamSizeShift : 4;
    std::uint8_t prgNvramSizeShift : 4;
    std::uint8_t chrRamSizeShift : 4;
    std::uint8_t chrNvramSizeShift : 4;
    std::uint8_t region : 2;
    std::uint8_t reserved0 : 6;
    std::uint8_t reserved[3];
};

struct RomHeader
{
    char         magic[4];
    std::uint8_t prgRomSize;
    std::uint8_t chrRomSize;
    std::uint8_t mirroring : 1;
    std::uint8_t battery : 1;
    std::uint8_t trainer : 1;
    std::uint8_t quadScreen : 1;
    std::uint8_t mapperLo : 4;
    std::uint8_t vs : 1;
    std::uint8_t playChoice : 1;
    std::uint8_t magicNes2 : 2;
    std::uint8_t mapperHi : 4;

    union
    {
        RomHeaderLegacy ines;
        RomHeaderNes2   nes2;
    };
};

} // namespace libnin

#endif
