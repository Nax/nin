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

#ifndef LIBNIN_AUDIO_H
#define LIBNIN_AUDIO_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>
#include <nin/nin.h>

namespace libnin
{

class HardwareInfo;
class Audio : private NonCopyable
{
public:
    Audio(const HardwareInfo& info);
    ~Audio();

    void setCallback(NINAUDIOCALLBACK callback, void* arg);
    void setTargetFrequency(std::uint32_t freq);

    void push(float sample);

private:
    static constexpr const int kOversampling       = 8;
    static constexpr const int kTargetFrequency    = 48000;
    static constexpr const int kTargetFrequencyRaw = kTargetFrequency * kOversampling;
    static constexpr const int kLowPassFilterWidth = 64;
    static constexpr const int kMaxRawSamples      = NIN_AUDIO_SAMPLE_SIZE * kOversampling + kLowPassFilterWidth;

    void resample();

    const HardwareInfo& _info;

    NINAUDIOCALLBACK _callback;
    void*            _callbackArg;
    std::uint32_t    _targetFrequency;
    std::uint32_t    _accumulator;
    float            _samplesRaw[kMaxRawSamples];
    float            _samples[NIN_AUDIO_SAMPLE_SIZE];
    std::uint16_t    _samplesCursor;
};

}; // namespace libnin

#endif
