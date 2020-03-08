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
#include <cmath>
#include <libnin/PPU.h>
#include <libnin/BusVideo.h>
#include <libnin/HardwareInfo.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/NMI.h>
#include <libnin/Video.h>

using namespace libnin;

static const uint16_t kHMask = 0x041f;

static std::uint16_t incX(std::uint16_t v)
{
    if ((v & 0x001f) == 31)
    {
        v &= ~0x001f;
        v ^= 0x0400;
    }
    else
        v++;
    return v;
}

static std::uint16_t incY(std::uint16_t v)
{
    std::uint16_t y;

    if ((v & 0x7000) != 0x7000)
        v += 0x1000;
    else
    {
        v &= ~0x7000;
        y = (v & 0x03E0) >> 5;
        if (y == 29)
        {
            y = 0;
            v ^= 0x0800;
        }
        else if (y == 31)
            y = 0;
        else
            y++;
        v = (v & ~0x03E0) | (y << 5);
    }

    return v;
}

static std::uint8_t bitswap8(std::uint8_t b)
{
    std::uint8_t v;

    v = 0;
    v |= (b & 0x80) >> 7;
    v |= (b & 0x40) >> 5;
    v |= (b & 0x20) >> 3;
    v |= (b & 0x10) >> 1;
    v |= (b & 0x08) << 1;
    v |= (b & 0x04) << 3;
    v |= (b & 0x02) << 5;
    v |= (b & 0x01) << 7;

    return v;
}

PPU::PPU(HardwareInfo& info, Memory& memory, NMI& nmi, BusVideo& busVideo, Mapper& mapper, Video& video)
: _info{info}
, _memory{memory}
, _nmi{nmi}
, _busVideo{busVideo}
, _mapper{mapper}
, _video{video}
, _t{}
, _v{}
, _x{}
, _scanline{}
, _cycle{}
, _latchName{}
, _latchAttr{}
, _latchTileLo{}
, _latchTileHi{}
, _shiftPatternLo{}
, _shiftPatternHi{}
, _shiftPaletteHi{}
, _shiftPaletteLo{}
, _oam2{}
, _oam2Index{}
, _latchSpriteBitmapLo{}
, _latchSpriteBitmapHi{}
, _latchSpriteBitmapAttr{}
, _latchSpriteBitmapX{}
, _zeroHit{}
, _zeroHitNext{}
, _maskEnableBackground{}
, _maskEnableBackgroundLeft{}
, _maskEnableSprites{}
, _maskEnableSpritesLeft{}
, _largeSprites{}
, _inhibitNmi{}
, _oamAddr{}
, _readBuf{}
, _latch{}
, _controller{}
, _scrollX{}
, _scrollY{}
, _w{}
, _zeroHitFlag{}
, _race0{}
, _race1{}
, _frameOdd{}
{

}

std::uint8_t PPU::regRead(std::uint16_t reg)
{
    std::uint8_t value;
    std::uint8_t mask;

    value = 0;
    mask = 0;

    switch (reg & 0x07)
    {
    case 0x00:
        break;
    case 0x01:
        break;
    case 0x02: // PPUSTATUS
        mask = 0xe0;
        if (_scanline == 241 && _cycle == 0) // Pre-set race
        {
            _inhibitNmi = 1;
        }
        else if (_scanline == 241 && _cycle == 1) // Same-set
        {
            _inhibitNmi = 1;
            value |= 0x80;
        }
        else if (_scanline == 241 && _cycle == 2) // Post-set
        {
            _nmi.unset(NMI_OCCURED);
            _nmi.ack();
            value |= 0x80;
        }
        else if (_nmi.check(NMI_OCCURED)) // Normal operation
        {
            value |= 0x80;
        }
        if (_nmi.check(NMI_OCCURED))
            value |= 0x80;
        _nmi.unset(NMI_OCCURED);
        if (_zeroHitFlag)
            value |= 0x40;
        _w = 0;
        break;
    case 0x03:
        break;
    case 0x04: // OAMDATA
        mask = 0xff;
        value = _memory.oam[_oamAddr];
        break;
    case 0x05:
        break;
    case 0x06:
        break;
    case 0x07: // PPUDATA
        mask = 0xff;
        if ((_v & 0x3f00) == 0x3f00)
        {
            value = _busVideo.read(_v);
            _readBuf = _busVideo.read(_v & 0x2fff);
        }
        else
        {
            value = _readBuf;
            _readBuf = _busVideo.read(_v);
        }
        _v += (_controller & 0x04) ? 32 : 1;
        break;
    }

    value = (value & mask) | (_latch & ~mask);
    _latch = value;
    return value;
}

void PPU::regWrite(std::uint16_t reg, std::uint8_t value)
{
    _latch = value;

    switch (reg & 0x07)
    {
    case 0x00: // PPUCTRL
        _controller = value;
        // Set base nametable
        _t &= ~0x0c00;
        _t |= ((value & 0x03) << 10);
        _largeSprites = ((value >> 5) & 1);
        if (value & 0x80)
            _nmi.set(NMI_OUTPUT);
        else
            _nmi.unset(NMI_OUTPUT);
        break;
    case 0x01:
        _maskEnableBackground = ((value & 0x08) >> 3);
        _maskEnableSprites = ((value & 0x10) >> 4);
        _maskEnableBackgroundLeft = !!(value & 0x02);
        _maskEnableSpritesLeft = !!(value & 0x04);
        break;
    case 0x02:
        break;
    case 0x03: // OAMADDR
        _oamAddr = value;
        break;
    case 0x04: // OAMDATA
        oamWrite(value);
        break;
    case 0x05: // PPUSCROLL
        if (!_w)
        {
            // Fine X
            _x = value & 0x07;

            // Coarse X
            _t &= ~(0x001f);
            _t |= ((value >> 3) & 0x1f);
        }
        else
        {
            // Fine Y
            _t &= ~(0x7000);
            _t |= (((uint16_t)value & 0x07) << 12);

            // Coarse Y
            _t &= ~(0x03e0);
            _t |= (((uint16_t)value >> 3) & 0x1f) << 5;
        }
        _w ^= 1;
        break;
    case 0x06: // PPUADDR
        if (!_w)
        {
            _t &= 0x00ff;
            _t |= (((uint16_t)value << 8) & 0x3fff);
        }
        else
        {
            _t &= 0xff00;
            _t |= (value & 0x00ff);
            _v = _t;
            _mapper.videoRead(_v);
        }
        _w ^= 1;
        break;
    case 0x07: //PPUDATA
        _busVideo.write(_v, value);
        _v += ((_controller & 0x04) ? 32 : 1);
        break;
    }
}

void PPU::oamWrite(std::uint8_t value)
{
    _memory.oam[_oamAddr++] = value;
}

bool PPU::tick(std::size_t cycles)
{
    bool newFrame{};

    while (cycles--)
    {
        if (_scanline < 240) scanline<false>();
        if (_scanline == 241 + _info.specs().vblank) scanline<true>();
        if (_scanline == 241 && _cycle == 1)
        {
            if (!_inhibitNmi)
                _nmi.set(NMI_OCCURED);
            _inhibitNmi = 0;
        }

        _cycle++;
        if (_cycle == 341)
        {
            _cycle = 0;
            _scanline++;
            if (_scanline == 242 + _info.specs().vblank)
            {
                _video.swap();
                _scanline = 0;
                newFrame = true;
                _frameOdd = !_frameOdd;
            }
        }
    }

    return newFrame;
}

template <bool prerender>
void PPU::scanline()
{
    uint16_t cycle;
    int isRendering;

    cycle = _cycle;
    isRendering = (_maskEnableBackground || _maskEnableSprites);

    if (_scanline == 0 && _maskEnableBackground && cycle == 0 && _frameOdd)
    {
        cycle++;
        _cycle++;
    }
    if (prerender && cycle == 1)
    {
        _nmi.unset(NMI_OCCURED);
        _zeroHitFlag = 0;
        _zeroHit = 0;
        _zeroHitNext = 0;
    }
    if (isRendering)
    {
        if (!prerender) spriteEvaluation(cycle);
        if ((cycle > 0 && cycle <= 256) || (cycle >= 321 && cycle <= 336))
        {
            bgFetch(cycle - 1);
        }
        if (!prerender && cycle >= 2 && cycle <= 257)
            emitPixel(cycle - 2);
        if ((cycle >= 9 && cycle <= 257 && ((cycle % 8) == 1)) || cycle == 329 || cycle == 337)
        {
            bgReload();
        }
        if (cycle == 256)
        {
            _v = incY(_v);
        }
        if (cycle == 257)
        {
            _v &= ~kHMask;
            _v |= (_t & kHMask);
        }
        if (prerender && (cycle >= 280 && cycle <= 304))
        {
            _v &= kHMask;
            _v |= (_t & ~kHMask);
        }
    }
}

void PPU::bgReload()
{
    _shiftPatternLo <<= 8;
    _shiftPatternHi <<= 8;
    _shiftPaletteLo <<= 8;
    _shiftPaletteHi <<= 8;
    _shiftPatternLo |= _latchTileLo;
    _shiftPatternHi |= _latchTileHi;
    _shiftPaletteLo |= (_latchAttr & 0x01) ? 0xff : 0x00;
    _shiftPaletteHi |= (_latchAttr & 0x02) ? 0xff : 0x00;
}

void PPU::bgFetch(std::uint16_t x)
{
    switch (x & 0x07)
    {
    case 0x01:
        _latchName = _busVideo.read(0x2000 | (_v & 0xfff));
        break;
    case 0x03:
        _latchAttr = _busVideo.read(0x23c0 | (_v & 0x0c00) | ((_v >> 4) & 0x38) | ((_v >> 2) & 0x07));
        if (_v & 0x02)
            _latchAttr >>= 2;
        if ((_v >> 5) & 0x02)
            _latchAttr >>= 4;
        break;
    case 0x05:
        _latchTileLo = _busVideo.read(((_controller & 0x10) ? 0x1000 : 0x0000) | _latchName << 4 | ((_v >> 12) & 0x07));
        break;
    case 0x07:
        _latchTileHi = _busVideo.read(((_controller & 0x10) ? 0x1000 : 0x0000) | _latchName << 4 | 0x08 | ((_v >> 12) & 0x07));
        _v = incX(_v);
        break;
    default:
        break;
    }
}

void PPU::emitPixel(std::uint16_t x)
{
    std::uint16_t addr;
    std::uint8_t shift;
    std::uint8_t spMask;
    std::uint8_t bgIndex;
    std::uint8_t spIndex;
    std::uint8_t palette;
    std::uint8_t color;

    bool bgEnabled = _maskEnableBackground && (x >= 8 || _maskEnableBackgroundLeft);
    bool spEnabled = _maskEnableSprites && (x >= 8 || _maskEnableSpritesLeft) && _scanline;

    shift = 15 - (x % 8);
    shift -= _x;

    if (bgEnabled)
    {
        bgIndex = (_shiftPatternLo >> shift) & 0x01;
        bgIndex |= (((_shiftPatternHi >> shift) & 0x01) << 1);

        palette = (_shiftPaletteLo >> shift) & 0x01;
        palette |= (((_shiftPaletteHi >> shift) & 0x01) << 1);

        addr = 0x3F00;
        if (bgIndex)
            addr = 0x3F00 | (palette << 2) | bgIndex;
        color = _busVideo.read(addr) & 0x3f;
    }
    else
    {
        bgIndex = 0;
        color = 0x3f;
    }

    if (spEnabled)
    {
        for (unsigned i = 0; i < 8; ++i)
        {
            if (x >= _latchSpriteBitmapX[i] && x < _latchSpriteBitmapX[i] + 8)
            {
                spMask = (0x80 >> (x - _latchSpriteBitmapX[i]));
                spIndex = !!(_latchSpriteBitmapLo[i] & spMask);
                spIndex |= ((!!(_latchSpriteBitmapHi[i] & spMask)) << 1);
                palette = _latchSpriteBitmapAttr[i] & 0x03;

                if (i == 0 && spIndex && bgIndex && _zeroHit && _maskEnableBackground && _maskEnableSprites && x != 255)
                    _zeroHitFlag = 1;

                if (spIndex && (!(_latchSpriteBitmapAttr[i] & 0x20) || !bgIndex))
                {
                    addr = 0x3F10 | (palette << 2) | spIndex;
                    color = _busVideo.read(addr) & 0x3f;
                    break;
                }
            }
        }
    }
    _video.write(_scanline * 256 + x, color);
}

void PPU::spriteEvaluation(std::uint16_t cycle)
{
    Sprite sprite;
    unsigned n;
    std::uint16_t addr;
    std::uint8_t tile;
    std::uint8_t y;
    std::uint16_t alt;
    std::uint8_t height;

    height = _largeSprites ? 16 : 8;

    if (cycle == 0)
    {
        _oam2Index = 0;
        _zeroHit = _zeroHitNext;
        _zeroHitNext = 0;
    }
    if (cycle >= 1 && cycle <= 64 && !(cycle & 0x01))
    {
        /* Clear OAM2 */
        n = (cycle - 1) / 2;
        _oam2[n / 4].raw[n % 4] = 0xff;
    }
    if (cycle >= 65 && cycle <= 192 && !(cycle & 0x01) && _oam2Index < 8)
    {
        /* Actual evaluation */
        n = (cycle - 65) / 2;
        memcpy(&sprite, _memory.oam + sizeof(Sprite) * n, sizeof(Sprite));
        if (_scanline >= sprite.y && _scanline < (std::uint16_t)sprite.y + height)
        {
            if (n == 0)
                _zeroHitNext = 1;
            _oam2[_oam2Index++] = sprite;
        }
    }
    if (cycle >= 257 && cycle <= 320 && ((cycle % 8) == 0))
    {
        n = (cycle - 257) / 8;

        if (n >= _oam2Index)
        {
            _latchSpriteBitmapX[n] = 0x00;
            _latchSpriteBitmapAttr[n] = 0x00;
            _latchSpriteBitmapLo[n] = 0x00;
            _latchSpriteBitmapHi[n] = 0x00;

            _mapper.videoRead(0x1000);
            _mapper.videoRead(0x1000);
        }
        else
        {
            y = _scanline - _oam2[n].y;
            tile = _oam2[n].tile;
            alt = 0;
            if (!_largeSprites && (_controller & 0x08))
            {
                alt = 0x1000;
            }
            if (_largeSprites && (tile & 1))
            {
                tile &= ~0x01;
                alt = 0x1000;
            }
            if (_oam2[n].yFlip)
            {
                y = height - y - 1;
            }
            if (_largeSprites && y > 7)
            {
                y &= 7;
                tile++;
            }
            addr = alt | (tile << 4) | y;

            _latchSpriteBitmapX[n] = _oam2[n].x;
            _latchSpriteBitmapAttr[n] = _oam2[n].raw[2];
            _latchSpriteBitmapLo[n] = _busVideo.read(addr);
            _latchSpriteBitmapHi[n] = _busVideo.read(addr | 0x08);

            if (_oam2[n].xFlip)
            {
                _latchSpriteBitmapLo[n] = bitswap8(_latchSpriteBitmapLo[n]);
                _latchSpriteBitmapHi[n] = bitswap8(_latchSpriteBitmapHi[n]);
            }
        }
    }
}
