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

#ifndef LIBNIN_HARDWARE_SPECS_H
#define LIBNIN_HARDWARE_SPECS_H 1

#include <cstdint>

namespace libnin
{

class HardwareSpecs
{
public:
    std::uint32_t clockRate;
    std::uint32_t frameCycles;
    std::uint32_t frameDelay;
    std::uint32_t cycleExtraIncrement;
    std::uint32_t firstVisibleScanline;
    std::uint16_t vblank;
    std::uint16_t apuFrameIrq;
    std::uint16_t apuFrameCycles[5];
    std::uint16_t apuNoisePeriod[16];
    std::uint16_t apuDmcPeriod[16];

    static const HardwareSpecs NTSC;
    static const HardwareSpecs PAL;
};

} // namespace libnin

#endif
