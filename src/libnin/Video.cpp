/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstring>
#include <libnin/Video.h>

using namespace libnin;

/* A R G B */
const uint32_t Video::kPalette[] = {
    0xff7c7c7c, 0xfffc0000, 0xffbc0000, 0xffbc2844,
    0xff840094, 0xff2000a8, 0xff0010a8, 0xff001488,
    0xff003050, 0xff007800, 0xff006800, 0xff005800,
    0xff584000, 0xff000000, 0xff000000, 0xff000000,
    0xffbcbcbc, 0xfff87800, 0xfff85800, 0xfffc4468,
    0xffcc00d8, 0xff5800e4, 0xff0038f8, 0xff105ce4,
    0xff007cac, 0xff00b800, 0xff00a800, 0xff44a800,
    0xff888800, 0xff000000, 0xff000000, 0xff000000,
    0xfff8f8f8, 0xfffcbc3c, 0xfffc8868, 0xfff87898,
    0xfff878f8, 0xff9858f8, 0xff5878f8, 0xff44a0fc,
    0xff00b8f8, 0xff18f8b8, 0xff54d858, 0xff98f858,
    0xffd8e800, 0xff787878, 0xff000000, 0xff000000,
    0xfffcfcfc, 0xfffce4a4, 0xfff8b8b8, 0xfff8b8d8,
    0xfff8b8f8, 0xffc0a4f8, 0xffb0d0f0, 0xffa8e0fc,
    0xff78d8f8, 0xff78f8d8, 0xffb8f8b8, 0xffd8f8b8,
    0xfffcfc00, 0xfff8d8f8, 0xff000000, 0xff000000,
};

Video::Video()
: _buffer0{}
, _buffer1{}
, _backBuffer{_buffer0}
, _frontBuffer{_buffer1}
{

}

void Video::swap()
{
    std::uint32_t* tmp;

    tmp = _backBuffer;
    _backBuffer = _frontBuffer;
    _frontBuffer = tmp;
    std::memset(_backBuffer, 0, sizeof(_buffer0));
}
