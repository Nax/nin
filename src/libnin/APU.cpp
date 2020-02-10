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

#include <libnin/libnin.h>
#include <libnin/APU.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/HardwareSpecs.h>

static constexpr const std::uint8_t kTriangleSequence[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static constexpr const std::uint8_t kPulseSequence[] = {
    0x02, /* 0 1 0 0 0 0 0 0 */
    0x06, /* 0 1 1 0 0 0 0 0 */
    0x1e, /* 0 1 1 1 1 0 0 0 */
    0xf9, /* 1 0 0 1 1 1 1 1 */
};

static constexpr const std::uint8_t kLengthCounterLookup[32] = {
    10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

APU::APU(const HardwareInfo& info, IRQ& irq, Mapper& mapper, Audio& audio)
: _info(info)
, _irq(irq)
, _mapper(mapper)
, _audio(audio)
, _triangle{}
, _pulse{}
, _noise{}
, _dmc{}
, _frameCounter{}
, _mode{}
, _irqInhibit{}
, _resetClock{}
{
    _noise.feedback = 1;
    _dmc.address = 0x8000;
}

std::uint8_t APU::regRead(std::uint16_t reg)
{
    uint8_t value;

    value = 0;
    switch (reg)
    {
    default:
        break;

    case 0x15:
        if (_pulse[0].length)                   value |= 0x01;
        if (_pulse[1].length)                   value |= 0x02;
        if (_triangle.length)                   value |= 0x04;
        if (_noise.length)                      value |= 0x08;
        if (_dmc.enabled)                       value |= 0x10;
        if (_irq.check(IRQ_APU_FRAME))          value |= 0x40;
        _irq.unset(IRQ_APU_FRAME);
        break;
    }

    return value;
}

void APU::regWrite(std::uint16_t reg, std::uint8_t value)
{
    unsigned i = (reg >> 2) & 1;

    switch (reg)
    {
    case 0x00: // Pulse Envelope
    case 0x04:
        _pulse[i].duty = kPulseSequence[value >> 6];
        loadEnvelope(_pulse[i].envelope, value);
        break;
    case 0x01: // Pulse Sweep
    case 0x05:
        _pulse[i].sweepEnable = !!(value & 0x80);
        _pulse[i].sweepPeriod = (value >> 4) & 0x07;
        _pulse[i].sweepNegate = !!(value & 0x08);
        _pulse[i].sweepShift = value & 0x07;
        _pulse[i].sweepReload = 1;
        pulseUpdateTarget(i);
        break;
    case 0x02: // Pulse Timer Lo
    case 0x06:
        _pulse[i].timerPeriod &= 0xff00;
        _pulse[i].timerPeriod |= value;
        pulseUpdateTarget(i);
        break;
    case 0x03: // Pulse Timer Hi
    case 0x07:
        _pulse[i].timerPeriod &= 0x00ff;
        _pulse[i].timerPeriod |= ((value & 0x7) << 8);
        _pulse[i].seqIndex = 0;
        if (_pulse[i].enabled)
            _pulse[i].length = kLengthCounterLookup[value >> 3];
        pulseUpdateTarget(i);
        _pulse[i].envelope.start = 1;
        break;
    case 0x08:
        if (value & 0x80)
            _triangle.control = 1;
        else
            _triangle.control = 0;
        _triangle.linearReload = value & 0x7f;
        break;
    case 0x09:
        break;
    case 0x0a: // Triangle Timer Lo
        _triangle.timerPeriod &= 0xff00;
        _triangle.timerPeriod |= value;
        break;
    case 0x0b: // Triangle Timer Hi
        _triangle.timerPeriod &= 0x00ff;
        _triangle.timerPeriod |= ((value & 0x7) << 8);
        if (_triangle.enabled) _triangle.length = kLengthCounterLookup[value >> 3];
        _triangle.linearReloadFlag = 1;
        break;
    case 0xc: // Noise Envelope
        loadEnvelope(_noise.envelope, value);
        break;
    case 0xe: // Noise Timer
        _noise.mode = !!(value & 0x80);
        _noise.timerPeriod = _info.specs().apuNoisePeriod[value & 0xf];
        break;
    case 0xf: // Noise Length
        if (_noise.enabled)
            _noise.length = kLengthCounterLookup[value >> 3];
        _noise.envelope.start = 1;
        break;
    case 0x10: // DMC Config
        _dmc.irqEnable = !!(value & 0x80);
        _dmc.loop = !!(value & 0x40);
        _dmc.timerPeriod = _info.specs().apuDmcPeriod[value & 0xf];
        break;
    case 0x11: // DMC Load
        _dmc.output = value & 0x7f;
        break;
    case 0x12: // DMC addr
        _dmc.address = 0xc000 | ((uint16_t)value << 6);
        break;
    case 0x13: // DMC Length
        _dmc.length = ((uint16_t)value << 4) | 1;
        break;
    case 0x15: // STATUS
        if (value & 0x01)
            _pulse[0].enabled = 1;
        else
        {
            _pulse[0].enabled = 0;
            _pulse[0].length = 0;
        }
        if (value & 0x02)
            _pulse[1].enabled = 1;
        else
        {
            _pulse[1].enabled = 0;
            _pulse[1].length = 0;
        }
        if (value & 0x04)
        {
            _triangle.enabled = 1;
        }
        else
        {
            _triangle.enabled = 0;
            _triangle.length = 0;
        }
        if (value & 0x08)
        {
            _noise.enabled = 1;
        }
        else
        {
            _noise.enabled = 0;
            _noise.length = 0;
        }
        if (value & 0x10)
        {
            _dmc.enabled = 1;
        }
        else
        {
            _dmc.enabled = 0;
        }
        break;
    case 0x17:
        _mode = !!(value & 0x80);
        _irqInhibit = !!(value & 0x40);
        if (_irqInhibit)
        {
            _irq.unset(IRQ_APU_FRAME);
        }
        if (_mode)
        {
            frameQuarter();
            frameHalf();
        }
        _resetClock = (_frameCounter & 0x01) ? 4 : 3;
        break;
    }
}

void APU::tick(std::size_t cycles)
{
    float sample;
    uint8_t triangleSample;
    uint8_t pulseSample[2];
    uint8_t noiseSample;
    uint8_t dmcSample;
    size_t maxApuCycle;

    maxApuCycle = _info.specs().apuFrameCycles[3 + _mode];
    while (cycles--)
    {
        if (_resetClock)
        {
            _resetClock--;
            if (_resetClock == 0)
                _frameCounter = 0;
        }

        tickTriangle();
        if (!(_frameCounter & 0x1))
        {
            tickPulse(0);
            tickPulse(1);
            tickNoise();
            tickDMC();
        }

        if (_frameCounter == _info.specs().apuFrameCycles[0]
            || _frameCounter == _info.specs().apuFrameCycles[1]
            || _frameCounter == _info.specs().apuFrameCycles[2]
            || _frameCounter == maxApuCycle)
        {
            frameQuarter();
        }

        if (_frameCounter == _info.specs().apuFrameCycles[1]
            || _frameCounter == maxApuCycle)
        {
            frameHalf();
        }

        if (_frameCounter >= maxApuCycle - 1)
        {
            if (!_mode && !_irqInhibit)
                _irq.set(IRQ_APU_FRAME);
        }

        if (_frameCounter == maxApuCycle + 1)
            _frameCounter = 1;
        else
            _frameCounter++;;

        /* Load the triangle sample */
        triangleSample = sampleTriangle();
        pulseSample[0] = samplePulse(0);
        pulseSample[1] = samplePulse(1);
        noiseSample = sampleNoise();
        dmcSample = sampleDMC();

        /* Emit the sample */
        sample = mix(triangleSample, pulseSample[0], pulseSample[1], noiseSample, dmcSample);
        _audio.push(sample);
    }
}

void APU::tickTriangle()
{
    if (_triangle.timerValue == 0)
    {
        _triangle.timerValue = _triangle.timerPeriod;
        if (_triangle.length && _triangle.linear)
        {
            _triangle.seqIndex++;
            _triangle.seqIndex &= 0x1f;
        }
    }
    else
    {
        _triangle.timerValue--;
    }
}

void APU::tickPulse(int n)
{
    ChannelPulse& ch = _pulse[n];

    if (ch.timerValue == 0)
    {
        ch.timerValue = ch.timerPeriod;
        ch.seqIndex++;
        ch.seqIndex &= 0x07;
    }
    else
    {
        ch.timerValue--;
    }
}

void APU::tickNoise()
{
    std::uint16_t tmp;
    std::uint16_t mask;

    if (_noise.timerValue)
        _noise.timerValue--;
    else
    {
        _noise.timerValue = _noise.timerPeriod;
        mask = _noise.mode ? 0x40 : 0x02;
        tmp = (!!(_noise.feedback & 0x01)) ^ (!!(_noise.feedback & mask));
        _noise.feedback >>= 1;
        _noise.feedback |= (tmp << 14);
    }
}

void APU::tickDMC()
{
    std::uint8_t bit;

    if (!_dmc.enabled)
        return;
    if (_dmc.timerValue)
    {
        _dmc.timerValue--;
        return;
    }
    _dmc.timerValue = _dmc.timerPeriod;
    if (_dmc.bitCount)
    {
        bit = _dmc.sampleBuffer & 0x01;
        _dmc.sampleBuffer >>= 1;
        _dmc.bitCount--;

        if (bit && _dmc.output <= 125)
            _dmc.output += 2;
        if (!bit && _dmc.output >= 2)
            _dmc.output -= 2;
    }
    if (!_dmc.bitCount && _dmc.length)
    {
        _dmc.length--;
        _dmc.sampleBuffer = dmcMemoryRead(_dmc.address);
        _dmc.bitCount = 8;
        _dmc.address++;
    }
}

void APU::frameHalf()
{
    frameHalfTriangle();
    frameHalfPulse(0);
    frameHalfPulse(1);
    frameHalfNoise();
}

void APU::frameHalfTriangle()
{
    if (_triangle.length && !_triangle.control)
        _triangle.length--;
}

void APU::frameHalfPulse(int n)
{
    ChannelPulse& ch = _pulse[n];

    if (ch.sweepEnable && ch.sweepValue == 0 && ch.sweepTarget < 0x800)
    {
        ch.timerPeriod = ch.sweepTarget;
        pulseUpdateTarget(n);
    }
    if (ch.sweepValue == 0 || ch.sweepReload)
    {
        ch.sweepValue = ch.sweepPeriod;
        ch.sweepReload = 0;
    }
    else
    {
        ch.sweepValue--;
    }

    if (ch.length && !ch.envelope.halt)
        ch.length--;
}

void APU::frameHalfNoise()
{
    if (_noise.length && !_noise.envelope.halt)
        _noise.length--;
}

void APU::frameQuarter()
{
    frameQuarterTriangle();

    tickEnvelope(_pulse[0].envelope);
    tickEnvelope(_pulse[1].envelope);
    tickEnvelope(_noise.envelope);
}

void APU::frameQuarterTriangle()
{
    if (_triangle.linearReloadFlag)
        _triangle.linear = _triangle.linearReload;
    else if (_triangle.linear)
        _triangle.linear--;
    if (!_triangle.control)
        _triangle.linearReloadFlag = 0;
}

std::uint8_t APU::sampleTriangle()
{
    return kTriangleSequence[_triangle.seqIndex];
}

std::uint8_t APU::samplePulse(int n)
{
    ChannelPulse& ch = _pulse[n];

    if (!ch.enabled || ch.timerPeriod < 8 || !ch.length || ch.sweepTarget >= 0x800)
        return 0;
    return (ch.duty & (1 << ch.seqIndex)) ? sampleEnvelope(ch.envelope) : 0;
}

std::uint8_t APU::sampleNoise()
{
    if (!_noise.enabled || (_noise.feedback & 0x01) || !_noise.length)
        return 0;
    return sampleEnvelope(_noise.envelope);
}

std::uint8_t APU::sampleDMC()
{
    return _dmc.output;
}

std::uint8_t APU::dmcMemoryRead(std::uint16_t addr)
{
    return _mapper.prg((addr & 0x6000) >> 13)[addr & 0x1fff];
}

float APU::mix(uint8_t triangle, uint8_t pulse1, uint8_t pulse2, uint8_t noise, uint8_t dmc)
{
    float fPulse;
    float fTND;

    if (pulse1 || pulse2)
    {
        fPulse = 95.88f / ((8128.f / ((float)pulse1 + (float)pulse2)) + 100.f);
    }
    else
        fPulse = 0.f;

    if (triangle || noise || dmc)
    {
        fTND = ((float)triangle / 8227.f);
        fTND += ((float)noise / 12241.f);
        fTND += ((float)dmc / 22638.f);
        fTND = 159.79f / ((1.f / fTND) + 100.f);
    }
    else
        fTND = 0.f;
    return fPulse + fTND;
}

void APU::loadEnvelope(Envelope& ev, uint8_t value)
{
    ev.halt = !!(value & 0x20);
    ev.constant = !!(value & 0x10);
    ev.volume = value & 0xf;
}

void APU::tickEnvelope(Envelope& ev)
{
    if (ev.start)
    {
        ev.start = 0;
        ev.decay = 15;
        ev.divider = ev.volume;
    }
    else
    {
        if (ev.divider)
            ev.divider--;
        else
        {
            ev.divider = ev.volume;
            if (ev.decay)
                ev.decay--;
            else if (ev.halt)
                ev.decay = 15;
        }
    }
}

std::uint8_t APU::sampleEnvelope(Envelope& ev)
{
    if (ev.constant)
        return ev.volume;
    return ev.decay;
}

void APU::pulseUpdateTarget(int n)
{
    ChannelPulse& ch = _pulse[n];
    std::uint16_t tmp;

    tmp = ch.timerPeriod >> ch.sweepShift;
    if (ch.sweepNegate)
        ch.sweepTarget = ch.timerPeriod - tmp - (1 - n);
    else
        ch.sweepTarget = ch.timerPeriod + tmp;
}
