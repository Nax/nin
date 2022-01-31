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

#ifndef LIBNIN_DISK_H
#define LIBNIN_DISK_H 1

#include <cstdint>
#include <cstdio>
#include <libnin/NonCopyable.h>

namespace libnin
{

class Disk : private NonCopyable
{
public:
    static constexpr const int Gap0            = 3538;
    static constexpr const int Gap1            = 122;
    static constexpr const int DiskSizeArchive = 65500;
    static constexpr const int DiskSize        = 0x14000;

    Disk();
    ~Disk();

    std::uint8_t sideCount() const { return _sideCount; }
    bool         inserted() const { return _inserted && !(_insertClock); }

    std::uint8_t read(std::uint32_t addr) const { return inserted() ? _dataSide[addr] : 0x00; }
    void         write(std::uint32_t addr, std::uint8_t value)
    {
        if (inserted())
        {
            _dataSide[addr] = value;
        }
    }

    void tick();
    void setSide(int diskSide);

    void load(std::FILE* file, int offset, std::uint8_t sideCount);

private:
    void loadSide(std::FILE* f, int offset, int side);

    std::uint8_t* _data;
    std::uint8_t* _dataSide;
    std::uint8_t  _sideCount;
    std::uint8_t  _side;
    std::uint32_t _insertClock;
    bool          _inserted;
};

} // namespace libnin

#endif
