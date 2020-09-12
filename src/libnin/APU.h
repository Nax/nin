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

#ifndef LIBNIN_APU_H
#define LIBNIN_APU_H 1

#include <cstddef>
#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class HardwareInfo;
class IRQ;
class Mapper;
class Audio;
class APU : private NonCopyable
{
public:
    APU(const HardwareInfo& info, IRQ& irq, Mapper& mapper, Audio& audio);

    std::uint8_t regRead(std::uint16_t reg);
    void         regWrite(std::uint16_t reg, std::uint8_t value);
    void         tick(std::size_t cycles);

private:
    struct Envelope
    {
        std::uint8_t volume : 4;
        std::uint8_t decay : 4;
        std::uint8_t divider : 4;
        std::uint8_t halt : 1;
        std::uint8_t constant : 1;
        std::uint8_t start : 1;
    };

    struct ChannelTriangle
    {
        std::uint16_t timerPeriod;
        std::uint16_t timerValue;
        std::uint8_t  seqIndex;
        std::uint8_t  linear;
        std::uint8_t  linearReload : 7;
        std::uint8_t  linearReloadFlag : 1;
        std::uint8_t  length;
        std::uint8_t  control : 1;
        std::uint8_t  enabled : 1;
    };

    struct ChannelPulse
    {
        std::uint16_t timerPeriod;
        std::uint16_t timerValue;
        std::uint8_t  seqIndex;
        std::uint8_t  length;
        std::uint8_t  duty;
        Envelope      envelope;
        std::uint8_t  sweepEnable : 1;
        std::uint8_t  sweepPeriod : 4;
        std::uint8_t  sweepValue : 4;
        std::uint8_t  sweepNegate : 1;
        std::uint8_t  sweepShift : 3;
        std::uint16_t sweepTarget;
        std::uint8_t  sweepReload : 1;
        std::uint8_t  enabled : 1;
    };

    struct ChannelNoise
    {
        std::uint16_t timerPeriod;
        std::uint16_t timerValue;
        std::uint16_t feedback;
        std::uint8_t  length;
        std::uint8_t  mode : 1;
        std::uint8_t  enabled : 1;
        Envelope      envelope;
    };

    struct ChannelDMC
    {
        std::uint16_t timerPeriod;
        std::uint16_t timerValue;
        std::uint16_t address;
        std::uint16_t length;
        std::uint8_t  output;
        std::uint8_t  irqEnable : 1;
        std::uint8_t  irq : 1;
        std::uint8_t  loop : 1;
        std::uint8_t  enabled : 1;
        std::uint8_t  sampleBuffer;
        std::uint8_t  bitCount : 4;
        std::uint8_t  silence : 1;
    };

    void tickTriangle();
    void tickPulse(int n);
    void tickNoise();
    void tickDMC();

    void frameQuarter();
    void frameQuarterTriangle();

    void frameHalf();
    void frameHalfTriangle();
    void frameHalfPulse(int n);
    void frameHalfNoise();

    std::uint8_t sampleTriangle();
    std::uint8_t samplePulse(int n);
    std::uint8_t sampleNoise();
    std::uint8_t sampleDMC();

    std::uint8_t dmcMemoryRead(std::uint16_t addr);

    float mix(uint8_t triangle, uint8_t pulse1, uint8_t pulse2, uint8_t noise, uint8_t dmc);

    void         loadEnvelope(Envelope& ev, std::uint8_t value);
    void         tickEnvelope(Envelope& ev);
    std::uint8_t sampleEnvelope(Envelope& ev);

    void pulseUpdateTarget(int n);

    const HardwareInfo& _info;
    IRQ&                _irq;
    Mapper&             _mapper;
    Audio&              _audio;

    ChannelTriangle _triangle;
    ChannelPulse    _pulse[2];
    ChannelNoise    _noise;
    ChannelDMC      _dmc;
    std::uint16_t   _frameCounter;
    std::uint8_t    _mode : 1;
    std::uint8_t    _irqInhibit : 1;
    std::uint8_t    _resetClock;
};

}; // namespace libnin

#endif
