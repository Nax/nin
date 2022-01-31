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

#include <cstring>
#include <libnin/Video.h>

using namespace libnin;

/* A R G B */
const uint32_t Video::kPalette[] = {
    0xff7c7c7c,
    0xfffc0000,
    0xffbc0000,
    0xffbc2844,
    0xff840094,
    0xff2000a8,
    0xff0010a8,
    0xff001488,
    0xff003050,
    0xff007800,
    0xff006800,
    0xff005800,
    0xff584000,
    0xff000000,
    0xff000000,
    0xff000000,
    0xffbcbcbc,
    0xfff87800,
    0xfff85800,
    0xfffc4468,
    0xffcc00d8,
    0xff5800e4,
    0xff0038f8,
    0xff105ce4,
    0xff007cac,
    0xff00b800,
    0xff00a800,
    0xff44a800,
    0xff888800,
    0xff000000,
    0xff000000,
    0xff000000,
    0xfff8f8f8,
    0xfffcbc3c,
    0xfffc8868,
    0xfff87898,
    0xfff878f8,
    0xff9858f8,
    0xff5878f8,
    0xff44a0fc,
    0xff00b8f8,
    0xff18f8b8,
    0xff54d858,
    0xff98f858,
    0xffd8e800,
    0xff787878,
    0xff000000,
    0xff000000,
    0xfffcfcfc,
    0xfffce4a4,
    0xfff8b8b8,
    0xfff8b8d8,
    0xfff8b8f8,
    0xffc0a4f8,
    0xffb0d0f0,
    0xffa8e0fc,
    0xff78d8f8,
    0xff78f8d8,
    0xffb8f8b8,
    0xffd8f8b8,
    0xfffcfc00,
    0xfff8d8f8,
    0xff000000,
    0xff000000,
};

Video::Video()
: _buffer0{}
, _buffer1{}
, _frontBuffer{_buffer0}
, _backBuffer{_buffer1}
, _frameChanged{}
{
}

void Video::swap()
{
    std::uint32_t* tmp;

    tmp          = _backBuffer;
    _backBuffer  = _frontBuffer;
    _frontBuffer = tmp;
    std::memset(_backBuffer, 0, sizeof(_buffer0));
    _frameChanged = true;
}
