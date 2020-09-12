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

#include <cmath>
#include <cstdio>
#include <cstring>
#include <libnin/BusVideo.h>
#include <libnin/HardwareInfo.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/NMI.h>
#include <libnin/PPU.h>
#include <libnin/Video.h>

using namespace libnin;

static constexpr std::uint8_t bitrev8(std::uint8_t v)
{
    alignas(64) constexpr const std::uint8_t kLookup[] =
        {
            0x00,
            0x80,
            0x40,
            0xc0,
            0x20,
            0xa0,
            0x60,
            0xe0,
            0x10,
            0x90,
            0x50,
            0xd0,
            0x30,
            0xb0,
            0x70,
            0xf0,
            0x08,
            0x88,
            0x48,
            0xc8,
            0x28,
            0xa8,
            0x68,
            0xe8,
            0x18,
            0x98,
            0x58,
            0xd8,
            0x38,
            0xb8,
            0x78,
            0xf8,
            0x04,
            0x84,
            0x44,
            0xc4,
            0x24,
            0xa4,
            0x64,
            0xe4,
            0x14,
            0x94,
            0x54,
            0xd4,
            0x34,
            0xb4,
            0x74,
            0xf4,
            0x0c,
            0x8c,
            0x4c,
            0xcc,
            0x2c,
            0xac,
            0x6c,
            0xec,
            0x1c,
            0x9c,
            0x5c,
            0xdc,
            0x3c,
            0xbc,
            0x7c,
            0xfc,
            0x02,
            0x82,
            0x42,
            0xc2,
            0x22,
            0xa2,
            0x62,
            0xe2,
            0x12,
            0x92,
            0x52,
            0xd2,
            0x32,
            0xb2,
            0x72,
            0xf2,
            0x0a,
            0x8a,
            0x4a,
            0xca,
            0x2a,
            0xaa,
            0x6a,
            0xea,
            0x1a,
            0x9a,
            0x5a,
            0xda,
            0x3a,
            0xba,
            0x7a,
            0xfa,
            0x06,
            0x86,
            0x46,
            0xc6,
            0x26,
            0xa6,
            0x66,
            0xe6,
            0x16,
            0x96,
            0x56,
            0xd6,
            0x36,
            0xb6,
            0x76,
            0xf6,
            0x0e,
            0x8e,
            0x4e,
            0xce,
            0x2e,
            0xae,
            0x6e,
            0xee,
            0x1e,
            0x9e,
            0x5e,
            0xde,
            0x3e,
            0xbe,
            0x7e,
            0xfe,
            0x01,
            0x81,
            0x41,
            0xc1,
            0x21,
            0xa1,
            0x61,
            0xe1,
            0x11,
            0x91,
            0x51,
            0xd1,
            0x31,
            0xb1,
            0x71,
            0xf1,
            0x09,
            0x89,
            0x49,
            0xc9,
            0x29,
            0xa9,
            0x69,
            0xe9,
            0x19,
            0x99,
            0x59,
            0xd9,
            0x39,
            0xb9,
            0x79,
            0xf9,
            0x05,
            0x85,
            0x45,
            0xc5,
            0x25,
            0xa5,
            0x65,
            0xe5,
            0x15,
            0x95,
            0x55,
            0xd5,
            0x35,
            0xb5,
            0x75,
            0xf5,
            0x0d,
            0x8d,
            0x4d,
            0xcd,
            0x2d,
            0xad,
            0x6d,
            0xed,
            0x1d,
            0x9d,
            0x5d,
            0xdd,
            0x3d,
            0xbd,
            0x7d,
            0xfd,
            0x03,
            0x83,
            0x43,
            0xc3,
            0x23,
            0xa3,
            0x63,
            0xe3,
            0x13,
            0x93,
            0x53,
            0xd3,
            0x33,
            0xb3,
            0x73,
            0xf3,
            0x0b,
            0x8b,
            0x4b,
            0xcb,
            0x2b,
            0xab,
            0x6b,
            0xeb,
            0x1b,
            0x9b,
            0x5b,
            0xdb,
            0x3b,
            0xbb,
            0x7b,
            0xfb,
            0x07,
            0x87,
            0x47,
            0xc7,
            0x27,
            0xa7,
            0x67,
            0xe7,
            0x17,
            0x97,
            0x57,
            0xd7,
            0x37,
            0xb7,
            0x77,
            0xf7,
            0x0f,
            0x8f,
            0x4f,
            0xcf,
            0x2f,
            0xaf,
            0x6f,
            0xef,
            0x1f,
            0x9f,
            0x5f,
            0xdf,
            0x3f,
            0xbf,
            0x7f,
            0xff,
        };

    return kLookup[v];
}

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

static std::uint16_t copyX(std::uint16_t v, std::uint16_t t)
{
    v &= ~kHMask;
    v |= (t & kHMask);
    return v;
}

static std::uint16_t copyY(std::uint16_t v, std::uint16_t t)
{
    v &= kHMask;
    v |= (t & ~kHMask);
    return v;
}

PPU::PPU(HardwareInfo& info, Memory& memory, NMI& nmi, BusVideo& busVideo, Mapper& mapper, Video& video)
: _info{info}
, _memory{memory}
, _nmi{nmi}
, _busVideo{busVideo}
, _mapper{mapper}
, _video{video}
, _handler{(Handler)&PPU::handleScan}
, _handler2{}
, _v{}
, _t{}
, _x{}
, _x2{}
, _w{}
, _prescan{}
, _spriteZeroNext{}
, _spriteZeroHit{}
, _oddFrame{}
, _dummySkip{}
, _nmiRace{}
, _nmiSup{}
, _flags{}
, _readBuf{}
, _latchNT{}
, _latchAT{}
, _latchLoBG{}
, _latchHiBG{}
, _shiftPatternLo{}
, _shiftPatternHi{}
, _shiftPaletteLo{}
, _shiftPaletteHi{}
, _clock{}
, _clockVideo{}
, _scanline{}
, _step{}
, _oamAddr{}
, _pixelBuffer{}
, _pixelBufferBits{}
{
}

std::uint8_t PPU::regRead(std::uint16_t reg)
{
    std::uint8_t value;

    value = 0;

    switch (reg & 0x07)
    {
    case 0x00:
        break;
    case 0x01:
        break;
    case 0x02: // PPUSTATUS
        if (_nmiRace)
            _nmiSup = true;
        if (_spriteZeroHit)
            value |= 0x40;
        if (_nmi.check(NMI_OCCURED))
            value |= 0x80;
        _nmi.unset(NMI_OCCURED);
        _w = false;
        break;
    case 0x03:
        break;
    case 0x04: // OAMDATA
        value = _memory.oam[_oamAddr];
        break;
    case 0x05:
        break;
    case 0x06:
        break;
    case 0x07: // PPUDATA
        if ((_v & 0x3f00) == 0x3f00)
        {
            value    = _busVideo.read(_v);
            _readBuf = _busVideo.read(_v & 0x2fff);
        }
        else
        {
            value    = _readBuf;
            _readBuf = _busVideo.read(_v);
        }
        _v += _flags.incrementY ? 32 : 1;
        break;
    }

    return value;
}

void PPU::regWrite(std::uint16_t reg, std::uint8_t value)
{
    switch (reg & 0x07)
    {
    case 0x00: // PPUCTRL
        // Set base nametable
        _t &= ~0x0c00;
        _t |= ((value & 0x03) << 10);

        _flags.incrementY           = !!(value & 0x04);
        _flags.altSpritePattern     = !!(value & 0x08);
        _flags.altBackgroundPattern = !!(value & 0x10);
        _flags.largeSprites         = !!(value & 0x20);

        if (value & 0x80)
            _nmi.set(NMI_OUTPUT);
        else
            _nmi.unset(NMI_OUTPUT);
        break;
    case 0x01:
        _flags.grayscale            = !!(value & 0x01);
        _flags.backgroundEnableLeft = !!(value & 0x02);
        _flags.spriteEnableLeft     = !!(value & 0x04);
        _flags.backgroundEnable     = !!(value & 0x08);
        _flags.spriteEnable         = !!(value & 0x10);
        _flags.emphasisRed          = !!(value & 0x20);
        _flags.emphasisGreen        = !!(value & 0x40);
        _flags.emphasisBlue         = !!(value & 0x80);
        _flags.rendering            = (_flags.backgroundEnable || _flags.spriteEnable);
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
        _w = !_w;
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
        _w = !_w;
        break;
    case 0x07: // PPUDATA
        _busVideo.write(_v, value);
        _v += _flags.incrementY ? 32 : 1;
        break;
    }
}

void PPU::oamWrite(std::uint8_t value)
{
    _memory.oam[_oamAddr++] = value;
}

void PPU::tick(std::size_t cycles)
{
    while (cycles--)
    {
        _handler = (Handler)(this->*_handler)();
        processPixel();
    }
}

void PPU::processPixel()
{
    std::uint8_t mask;

    if (_pixelBufferBits & 0x01)
    {
        mask = _flags.grayscale ? 0x30 : 0x3f;
        _video.write(_clockVideo, _pixelBuffer & mask);
        _clockVideo++;
    }
    _pixelBufferBits >>= 1;
    _pixelBuffer >>= 8;
}

PPU::Handler PPU::handleWait()
{
    if (!_clock)
        return _handler2;
    _clock--;
    return &PPU::handleWait;
}

PPU::Handler PPU::handleVBlank0()
{
    _nmiRace = true;
    return &PPU::handleVBlank1;
}

PPU::Handler PPU::handleVBlank1()
{
    //if (!_nmiSup)
    _nmi.set(NMI_OCCURED);
    _nmiRace = false;
    _nmiSup  = false;
    _video.swap();
    _clockVideo = 0;
    return wait(341 * 20 - 1, (Handler)&PPU::handlePreScan);
}

PPU::Handler PPU::handlePreScan()
{
    _nmi.unset(NMI_OCCURED);
    _prescan        = true;
    _spriteZeroNext = false;
    _spriteZeroHit  = false;
    for (int i = 0; i < 8; ++i)
    {
        _shiftSpriteHi[i] = 0x00;
        _shiftSpriteLo[i] = 0x00;
    }
    _step = 0;
    return wait(255, (Handler)&PPU::handlePreScanReloadX);
}

PPU::Handler PPU::handlePreScanReloadX()
{
    if (_flags.rendering)
    {
        _v = copyX(_v, _t);
    }
    _step = 0;
    return wait(22, (Handler)&PPU::handlePreScanReloadY);
}

PPU::Handler PPU::handlePreScanReloadY()
{
    if (_flags.rendering)
    {
        _v = copyY(_v, _t);
    }
    if (_step == 24)
    {
        _step = 0;
        return wait(16, &PPU::handleNextNT0);
    }
    _step++;
    return &PPU::handlePreScanReloadY;
}

PPU::Handler PPU::handleScan()
{
    _step = 0;
    return &PPU::handleScanNT0;
}

PPU::Handler PPU::handleScanNT0()
{
    emitPixel();
    return (Handler)&PPU::handleScanNT1;
}

PPU::Handler PPU::handleScanNT1()
{
    emitPixel();
    fetchNT();
    return (Handler)&PPU::handleScanAT0;
}

PPU::Handler PPU::handleScanAT0()
{
    emitPixel();
    return (Handler)&PPU::handleScanAT1;
}

PPU::Handler PPU::handleScanAT1()
{
    emitPixel();
    fetchAT();
    return (Handler)&PPU::handleScanLoBG0;
}

PPU::Handler PPU::handleScanLoBG0()
{
    emitPixel();
    return (Handler)&PPU::handleScanLoBG1;
}

PPU::Handler PPU::handleScanLoBG1()
{
    emitPixel();
    fetchLoBG();
    return (Handler)&PPU::handleScanHiBG0;
}

PPU::Handler PPU::handleScanHiBG0()
{
    emitPixel();
    return (Handler)&PPU::handleScanHiBG1;
}

PPU::Handler PPU::handleScanHiBG1()
{
    emitPixel();
    fetchHiBG();
    shiftReload();

    /* Increment X */
    if (_flags.rendering)
    {
        _v = incX(_v);
    }

    _step++;
    if (_step < 32)
    {
        return &PPU::handleScanNT0;
    }
    if (_flags.rendering)
    {
        _v = copyX(_v, _t);
        _v = incY(_v);
    }
    _step = 0;
    return &PPU::handleScanSpriteEval;
}

PPU::Handler PPU::handleScanSpriteEval()
{
    if (_flags.rendering)
    {
        spriteEvaluation();
        spriteFetch();
    }
    return wait(63, (Handler)&PPU::handleNextNT0);
}

PPU::Handler PPU::handleNextNT0()
{
    return (Handler)&PPU::handleNextNT1;
}

PPU::Handler PPU::handleNextNT1()
{
    fetchNT();
    return (Handler)&PPU::handleNextAT0;
}

PPU::Handler PPU::handleNextAT0()
{
    return (Handler)&PPU::handleNextAT1;
}

PPU::Handler PPU::handleNextAT1()
{
    fetchAT();
    return (Handler)&PPU::handleNextLoBG0;
}

PPU::Handler PPU::handleNextLoBG0()
{
    return (Handler)&PPU::handleNextLoBG1;
}

PPU::Handler PPU::handleNextLoBG1()
{
    fetchLoBG();
    return (Handler)&PPU::handleNextHiBG0;
}

PPU::Handler PPU::handleNextHiBG0()
{
    return (Handler)&PPU::handleNextHiBG1;
}

PPU::Handler PPU::handleNextHiBG1()
{
    if (_flags.rendering)
    {
        fetchHiBG();
        _v = incX(_v);
        shiftReload();
    }

    if (_step)
        return &PPU::handleNextDummy0;
    _step = 1;
    return &PPU::handleNextNT0;
}

PPU::Handler PPU::handleNextDummy0()
{
    return &PPU::handleNextDummy1;
}

PPU::Handler PPU::handleNextDummy1()
{
    _dummySkip = (_oddFrame && _flags.backgroundEnable);
    fetchNT();
    return &PPU::handleNextDummy2;
}

PPU::Handler PPU::handleNextDummy2()
{
    return &PPU::handleNextDummy3;
}

PPU::Handler PPU::handleNextDummy3()
{
    fetchNT();

    if (_prescan)
    {
        _prescan  = false;
        _scanline = 0;
        _step     = 0;
        _oddFrame = !_oddFrame;
        return _dummySkip ? &PPU::handleScanNT0 : &PPU::handleScan;
    }
    if (_scanline + 1 < 240)
    {
        _scanline++;
        return &PPU::handleScan;
    }
    _scanline = 0;
    return wait(341, &PPU::handleVBlank0);
}

PPU::Handler PPU::wait(std::uint32_t cycles, Handler next)
{
    _clock    = cycles - 1;
    _handler2 = next;

    return &PPU::handleWait;
}

PPU::Handler PPU::dummy()
{
    Handler h;

    if (_oddFrame && (_flags.backgroundEnable))
    {
        h = &PPU::handleScanNT0;
    }
    else
    {
        h = &PPU::handleScan;
    }
    _oddFrame = !_oddFrame;
    return h;
}

void PPU::fetchNT()
{
    _latchNT = _busVideo.read(0x2000 | (_v & 0xfff));
}

void PPU::fetchAT()
{
    _latchAT = _busVideo.read(0x23c0 | (_v & 0x0c00) | ((_v >> 4) & 0x38) | ((_v >> 2) & 0x07));
    if (_v & 0x02)
        _latchAT >>= 2;
    if ((_v >> 5) & 0x02)
        _latchAT >>= 4;
}

void PPU::fetchLoBG()
{
    _latchLoBG = _busVideo.read((_flags.altBackgroundPattern ? 0x1000 : 0x0000) | _latchNT << 4 | ((_v >> 12) & 0x07));
}

void PPU::fetchHiBG()
{
    _latchHiBG = _busVideo.read((_flags.altBackgroundPattern ? 0x1000 : 0x0000) | _latchNT << 4 | 0x08 | ((_v >> 12) & 0x07));
}

void PPU::spriteEvaluation()
{
    std::uint16_t y;
    std::uint8_t  spriteHeight;

    _oam2Count      = 0;
    _spriteZeroNext = false;

    std::memset(_oam2, 0xff, 32);
    spriteHeight = _flags.largeSprites ? 16 : 8;
    for (int i = 0; i < 64; ++i)
    {
        y = _memory.oam[4 * i];
        if (_scanline >= y && _scanline < y + spriteHeight)
        {
            // Sprite y-hit
            if (i == 0)
            {
                _spriteZeroNext = true;
            }
            memcpy(_oam2[_oam2Count].raw, _memory.oam + i * 4, 4);
            _oam2Count++;
            if (_oam2Count == 8)
                break;
        }
    }
}

void PPU::spriteFetch()
{
    std::uint16_t y;
    std::uint16_t tile;
    std::uint16_t nametable;
    std::uint16_t addr;

    int i;

    for (i = 0; i < _oam2Count; ++i)
    {
        y         = _scanline - _oam2[i].y;
        tile      = _oam2[i].tile;
        nametable = 0;

        if (_oam2[i].yFlip)
        {
            if (_flags.largeSprites)
                y = 15 - y;
            else
                y = 7 - y;
        }
        if (!_flags.largeSprites && _flags.altSpritePattern)
        {
            nametable = 0x1000;
        }
        if (_flags.largeSprites && tile & 0x01)
        {
            tile &= ~0x01;
            nametable = 0x1000;
        }
        if (y > 7)
        {
            y &= 7;
            tile++;
        }

        addr = nametable | (tile << 4) | y;

        _shiftSpriteX[i]    = _oam2[i].x;
        _shiftSpriteAttr[i] = _oam2[i].raw[2];
        _shiftSpriteLo[i]   = _busVideo.read(addr | 0x00);
        _shiftSpriteHi[i]   = _busVideo.read(addr | 0x08);
    }

    for (; i < 8; ++i)
    {
        _shiftSpriteX[i]    = 0;
        _shiftSpriteAttr[i] = 0;
        _shiftSpriteLo[i]   = 0;
        _shiftSpriteHi[i]   = 0;

        _mapper.videoRead(0x1000);
        _mapper.videoRead(0x1000);
    }
}

void PPU::emitPixel()
{
    std::uint8_t color;
    std::uint8_t bg;
    std::uint8_t sprite;
    //std::uint8_t mask;

    if (_flags.backgroundEnable && (_step || _flags.backgroundEnableLeft))
    {
        bg = pixelBackground();
    }
    else
    {
        bg = 0x00;
    }

    if (_flags.spriteEnable && (_step || _flags.spriteEnableLeft))
    {
        sprite = pixelSprite(bg);
    }
    else
    {
        sprite = 0x00;
    }
    _x2 = (_x2 + 1) & 0x07;

    //mask = _flags.grayscale ? 0x30 : 0x3f;
    color = _memory.palettes[sprite ? (0x10 | sprite) : bg];

    _pixelBuffer |= ((std::uint32_t)color << 16);
    _pixelBufferBits |= (0x01 << 2);
}

std::uint8_t PPU::pixelBackground()
{
    std::uint8_t shift;
    std::uint8_t pattern{};
    std::uint8_t palette{};
    std::uint8_t paletteIdx{};

    shift = _x + _x2;

    pattern |= ((_shiftPatternLo >> shift) & 0x01);
    pattern |= (((_shiftPatternHi >> shift) & 0x01) << 1);
    palette |= ((_shiftPaletteLo >> shift) & 0x01);
    palette |= (((_shiftPaletteHi >> shift) & 0x01) << 1);

    if (pattern)
    {
        paletteIdx = (palette << 2) | pattern;
    }

    return paletteIdx;
}

std::uint8_t PPU::pixelSprite(std::uint8_t bg)
{
    std::uint16_t scanX;
    std::uint16_t spriteX;
    std::uint8_t  pattern;
    std::uint8_t  shift;

    scanX = _step * 8 + _x2;

    for (int i = 0; i < 8; ++i)
    {
        spriteX = _shiftSpriteX[i];
        if (scanX >= spriteX && scanX < spriteX + 8)
        {
            shift = scanX - spriteX;
            if (!(_shiftSpriteAttr[i] & 0x40)) // X Flip
            {
                shift = 7 - shift;
            }
            pattern = 0;
            pattern |= (_shiftSpriteLo[i] >> shift) & 0x01;
            pattern |= ((_shiftSpriteHi[i] >> shift) & 0x01) << 1;
            if (pattern)
            {
                if (_spriteZeroNext && i == 0 && bg && !(_step == 31 && _x2 == 7))
                {
                    _spriteZeroHit = true;
                }
                if (bg && _shiftSpriteAttr[i] & 0x20)
                {
                    return 0;
                }
                return ((_shiftSpriteAttr[i] & 0x03) << 2) | pattern;
            }
        }
    }

    return 0;
}

void PPU::shiftReload()
{
    _shiftPatternLo >>= 8;
    _shiftPatternHi >>= 8;
    _shiftPaletteLo >>= 8;
    _shiftPaletteHi >>= 8;

    _shiftPatternLo |= (std::uint16_t)bitrev8(_latchLoBG) << 8;
    _shiftPatternHi |= (std::uint16_t)bitrev8(_latchHiBG) << 8;
    _shiftPaletteLo |= (_latchAT & 0x01) ? 0xff00 : 0x0000;
    _shiftPaletteHi |= (_latchAT & 0x02) ? 0xff00 : 0x0000;
}
