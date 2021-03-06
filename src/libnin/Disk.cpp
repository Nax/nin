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

#include <libnin/Disk.h>

using namespace libnin;

Disk::Disk()
: _data{}
, _dataSide{}
, _sideCount{}
, _side{}
, _insertClock{}
, _inserted{}
{
}

Disk::~Disk()
{
    delete[] _data;
}

void Disk::tick()
{
    if (_insertClock)
        _insertClock--;
}

void Disk::setSide(int diskSide)
{
    if (diskSide < 0 || diskSide >= _sideCount)
    {
        _inserted = false;
        return;
    }
    _side        = std::uint8_t(diskSide);
    _dataSide    = _data + std::uintptr_t(DiskSize) * _side;
    _insertClock = 1000000;
}

void Disk::load(std::FILE* f, int offset, std::uint8_t sideCount)
{
    _sideCount = sideCount;
    delete[] _data;
    _data = new std::uint8_t[(std::size_t)DiskSize * _sideCount]();

    for (std::uint8_t i = 0; i < _sideCount; ++i)
    {
        loadSide(f, offset, i);
    }

    _dataSide = _data;
    _inserted = true;
}

void Disk::loadSide(std::FILE* f, int fileOffset, int side)
{
    std::uint8_t* dst;
    std::uint32_t offset;
    std::uint16_t fileSize;
    std::uint8_t  fileCount;

    dst = _data + (std::size_t)DiskSize * side;
    std::fseek(f, fileOffset + side * DiskSizeArchive, SEEK_SET);

    /* First, large gap */
    offset          = Gap0;
    dst[offset - 1] = 0x80;

    /* Load block 1 */
    std::fread(dst + offset, 0x38, 1, f);
    offset += 0x3a;
    offset += Gap1;
    dst[offset - 1] = 0x80;

    /* Load block 2 */
    std::fread(dst + offset, 0x02, 1, f);
    fileCount = dst[offset + 1];
    offset += 0x04;

    for (int i = 0; i < fileCount; ++i)
    {
        offset += Gap1;
        dst[offset - 1] = 0x80;

        /* Block 3 */
        std::fread(dst + offset, 0x10, 1, f);
        fileSize = *(std::uint16_t*)(dst + offset + 13);
        offset += 0x12;
        offset += Gap1;
        dst[offset - 1] = 0x80;

        /* Block 4 */
        std::fread(dst + offset, (std::size_t)fileSize + 1, 1, f);
        offset += (fileSize + 3);
    }
}
