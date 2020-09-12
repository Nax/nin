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

#ifndef LIBNIN_MAPPER_DISK_SYSTEM_H
#define LIBNIN_MAPPER_DISK_SYSTEM_H 1

#include <cstdint>

namespace libnin
{

struct MapperDiskSystem
{
    std::uint32_t headPos;
    std::uint16_t delay;
    std::uint16_t irqReloadValue;
    std::uint16_t irqTimer;
    std::uint8_t  extPort;
    std::uint8_t  latchRead;
    std::uint8_t  latchWrite;
    std::uint8_t  sideCount;

    bool motor : 1;
    bool noScan : 1;
    bool inData : 1;
    bool write : 1;
    bool irqEnabledTransfer : 1;
    bool irqEnabledTimer : 1;
    bool irqReloadFlag : 1;
    bool transfered : 1;
    bool scanning : 1;
    bool skippedGap : 1;
    bool endOfDisk : 1;
    bool transmitCRC : 1;
};

} // namespace libnin

#endif
