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

#ifndef LIBNIN_VIDEO_H
#define LIBNIN_VIDEO_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class Video : private NonCopyable
{
public:
    Video();

    const std::uint32_t* front() const { return _frontBuffer; }

    void write(std::uint32_t pos, std::uint8_t color) { _backBuffer[pos] = kPalette[color]; }
    void swap();
    bool changed()
    {
        bool tmp      = _frameChanged;
        _frameChanged = false;
        return tmp;
    }

private:
    static const std::uint32_t kPalette[];

    std::uint32_t  _buffer0[256 * 240];
    std::uint32_t  _buffer1[256 * 240];
    std::uint32_t* _frontBuffer;
    std::uint32_t* _backBuffer;
    bool           _frameChanged;
};

}; // namespace libnin

#endif
