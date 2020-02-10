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

#ifndef LIBNIN_PPU_H
#define LIBNIN_PPU_H 1

#include <cstddef>
#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class HardwareInfo;
class Memory;
class NMI;
class BusVideo;
class Mapper;
class Video;
class PPU : private NonCopyable
{
public:
    PPU(HardwareInfo& info, Memory& memory, NMI& nmi, BusVideo& busVideo, Mapper& mapper, Video& video);

    std::uint8_t    regRead(std::uint16_t reg);
    void            regWrite(std::uint16_t reg, std::uint8_t value);
    void            oamWrite(std::uint8_t value);
    bool            tick(std::size_t cycles);

private:
    union Sprite
    {
        std::uint8_t raw[4];
        struct
        {
            std::uint8_t y;
            std::uint8_t tile;
            std::uint8_t palette:2;
            std::uint8_t reserved:3;
            std::uint8_t back:1;
            std::uint8_t xFlip:1;
            std::uint8_t yFlip:1;
            std::uint8_t x;
        };
    };

    template <bool prerender>
    void scanline();

    void bgReload();
    void bgFetch(std::uint16_t x);
    void emitPixel(std::uint16_t x);
    void spriteEvaluation(std::uint16_t cycle);

    HardwareInfo&   _info;
    Memory&         _memory;
    NMI&            _nmi;
    BusVideo&       _busVideo;
    Mapper&         _mapper;
    Video&          _video;

    std::uint16_t   _t;
    std::uint16_t   _v;
    std::uint8_t    _x;
    std::uint16_t   _scanline;
    std::uint16_t   _cycle;
    std::uint8_t    _latchName;
    std::uint8_t    _latchAttr;
    std::uint8_t    _latchTileLo;
    std::uint8_t    _latchTileHi;
    std::uint16_t   _shiftPatternLo;
    std::uint16_t   _shiftPatternHi;
    std::uint16_t   _shiftPaletteHi;
    std::uint16_t   _shiftPaletteLo;
    Sprite          _oam2[8];
    std::uint8_t    _oam2Index;
    std::uint8_t    _latchSpriteBitmapLo[8];
    std::uint8_t    _latchSpriteBitmapHi[8];
    std::uint8_t    _latchSpriteBitmapAttr[8];
    std::uint8_t    _latchSpriteBitmapX[8];
    unsigned        _zeroHit:1;
    unsigned        _zeroHitNext:1;
    unsigned        _maskEnableBackground:1;
    unsigned        _maskEnableBackgroundLeft:1;
    unsigned        _maskEnableSprites:1;
    unsigned        _maskEnableSpritesLeft:1;
    unsigned        _largeSprites:1;
    std::uint8_t    _inhibitNmi:1;
    std::uint8_t    _oamAddr;
    std::uint8_t    _readBuf;
    std::uint8_t    _latch;
    std::uint8_t    _controller;
    std::uint8_t    _scrollX;
    std::uint8_t    _scrollY;
    std::uint8_t    _w:1;
    std::uint8_t    _zeroHitFlag:1;
    std::uint8_t    _race0:1;
    std::uint8_t    _race1:1;
    bool            _frameOdd:1;
};

};

#endif
