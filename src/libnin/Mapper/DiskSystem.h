/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
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
#include <libnin/Mapper.h>

namespace libnin
{

class MapperDiskSystem : public Mapper
{
public:
    void         handleInit();
    void         handleTick();
    std::uint8_t handleRead(std::uint16_t addr);
    void         handleWrite(std::uint16_t addr, std::uint8_t value);

private:
    std::uint32_t _headPos;
    std::uint16_t _delay;
    std::uint16_t _irqReloadValue;
    std::uint16_t _irqTimer;
    std::uint8_t  _extPort;
    std::uint8_t  _latchRead;
    std::uint8_t  _latchWrite;
    std::uint8_t  _sideCount;

    bool _motor : 1;
    bool _noScan : 1;
    bool _inData : 1;
    bool _write : 1;
    bool _irqEnabledTransfer : 1;
    bool _irqEnabledTimer : 1;
    bool _irqReloadFlag : 1;
    bool _transfered : 1;
    bool _scanning : 1;
    bool _skippedGap : 1;
    bool _endOfDisk : 1;
    bool _transmitCRC : 1;
};

} // namespace libnin

#endif
