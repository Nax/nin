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

#ifndef LIBNIN_PPU_H
#define LIBNIN_PPU_H 1

#include <cstddef>
#include <cstdint>
#include <libnin/MemberStateHelper.h>
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

    std::uint8_t regRead(std::uint16_t reg);
    void         regWrite(std::uint16_t reg, std::uint8_t value);
    void         oamWrite(std::uint8_t value);

    void tick(std::size_t cycles);
    void processPixel();

private:
    union Sprite
    {
        std::uint8_t raw[4];
        struct
        {
            std::uint8_t y;
            std::uint8_t tile;
            std::uint8_t palette : 2;
            std::uint8_t reserved : 3;
            std::uint8_t back : 1;
            std::uint8_t xFlip : 1;
            std::uint8_t yFlip : 1;
            std::uint8_t x;
        };
    };

    struct Flags
    {
        bool incrementY : 1;
        bool altSpritePattern : 1;
        bool altBackgroundPattern : 1;
        bool largeSprites : 1;
        bool backgroundEnable : 1;
        bool backgroundEnableLeft : 1;
        bool spriteEnable : 1;
        bool spriteEnableLeft : 1;
        bool rendering : 1;
        bool grayscale : 1;
        bool emphasisRed : 1;
        bool emphasisGreen : 1;
        bool emphasisBlue : 1;
    };

    using Handler = MemberStateHelper<PPU>;

    Handler handleWait();
    Handler handleVBlank0();
    Handler handleVBlank1();
    Handler handleVBlank2();
    Handler handleVBlank3();

    Handler handlePreScan0();
    Handler handlePreScan1();
    Handler handlePreScanReloadX();
    Handler handlePreScanReloadY();
    Handler handleScan();
    Handler handleScanNT0();
    Handler handleScanNT1();
    Handler handleScanAT0();
    Handler handleScanAT1();
    Handler handleScanLoBG0();
    Handler handleScanLoBG1();
    Handler handleScanHiBG0();
    Handler handleScanHiBG1();
    Handler handleScanSpriteEval();
    Handler handleNextNT0();
    Handler handleNextNT1();
    Handler handleNextAT0();
    Handler handleNextAT1();
    Handler handleNextLoBG0();
    Handler handleNextLoBG1();
    Handler handleNextHiBG0();
    Handler handleNextHiBG1();
    Handler handleNextDummy0();
    Handler handleNextDummy1();
    Handler handleNextDummy2();
    Handler handleNextDummy3();

    Handler wait(std::uint32_t cycles, Handler next);
    Handler dummy();

    void fetchNT();
    void fetchAT();
    void fetchLoBG();
    void fetchHiBG();

    void spriteEvaluation();
    void spriteFetch();

    void         emitPixel();
    std::uint8_t pixelBackground();
    std::uint8_t pixelSprite(std::uint8_t bg);

    void shiftReload();

    HardwareInfo& _info;
    Memory&       _memory;
    NMI&          _nmi;
    BusVideo&     _busVideo;
    Mapper&       _mapper;
    Video&        _video;

    Handler _handler;
    Handler _handler2;

    std::uint16_t _v;
    std::uint16_t _t;
    std::uint8_t  _x;
    std::uint8_t  _x2;
    bool          _w : 1;
    bool          _prescan : 1;
    bool          _spriteZeroNext : 1;
    bool          _spriteZeroHit : 1;
    bool          _oddFrame : 1;
    bool          _dummySkip : 1;
    bool          _nmiRace : 1;
    bool          _nmiRace2 : 1;
    bool          _nmiRace3 : 1;
    bool          _nmiSup : 1;

    Flags        _flags;
    Sprite       _oam2[8];
    std::uint8_t _oam2Count;

    std::uint8_t _readBuf;
    std::uint8_t _latchNT;
    std::uint8_t _latchAT;
    std::uint8_t _latchLoBG;
    std::uint8_t _latchHiBG;

    std::uint16_t _shiftPatternLo;
    std::uint16_t _shiftPatternHi;
    std::uint16_t _shiftPaletteLo;
    std::uint16_t _shiftPaletteHi;

    std::uint8_t _shiftSpriteX[8];
    std::uint8_t _shiftSpriteAttr[8];
    std::uint8_t _shiftSpriteLo[8];
    std::uint8_t _shiftSpriteHi[8];

    std::uint32_t _clock;
    std::uint32_t _clockVideo;
    std::uint8_t  _scanline;
    std::uint8_t  _step;
    std::uint8_t  _oamAddr;

    std::uint32_t _pixelBuffer;
    std::uint8_t  _pixelBufferBits;
};

} // namespace libnin

#endif
