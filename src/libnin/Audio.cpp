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

#include <cmath>
#include <cstring>
#include <libnin/Audio.h>
#include <libnin/HardwareInfo.h>

using namespace libnin;

Audio::Audio(const HardwareInfo& info)
: _info(info)
, _callback(nullptr)
, _callbackArg(nullptr)
, _targetFrequency(0)
, _accumulator(0)
, _samplesRaw{}
, _samplesCursor{kLowPassFilterWidth / 2}
{
}

Audio::~Audio()
{
}

void Audio::setCallback(NINAUDIOCALLBACK callback, void* arg)
{
    _callback    = callback;
    _callbackArg = arg;
}

void Audio::setTargetFrequency(std::uint32_t freq)
{
    _targetFrequency = freq;
}

void Audio::push(float sample)
{
    /* We push samples into a ring buffer */
    //sample = loPass(sample);
    _accumulator += kTargetFrequencyRaw;
    if (_accumulator >= _info.specs().clockRate)
    {
        //sample = hiPass(sample);
        _accumulator -= _info.specs().clockRate;
        _samplesRaw[_samplesCursor++] = sample;
        if (_samplesCursor >= kMaxRawSamples)
        {
            resample();
            std::memmove(_samplesRaw, _samplesRaw + NIN_AUDIO_SAMPLE_SIZE * kOversampling, kLowPassFilterWidth * sizeof(float));
            _samplesCursor = kLowPassFilterWidth;
            if (_callback)
                _callback(_callbackArg, _samples);
        }
    }
}

void Audio::resample()
{
    static const float kImpulse[kLowPassFilterWidth] = {
        0.f, 0.f, -0.000527072017573503f, -0.00110984739000885f, -0.00170674802675828f, -0.00227100929579603f, -0.00275226865376297f, -0.00309836514905645f, -0.00325728724657753f, -0.00317919910449467f, -0.00281847067702838f, -0.00213563505184364f, -0.00109919737827612f, 0.000312776387504284f, 0.00211135600758337f, 0.00429578455840068f, 0.00685290133382793f, 0.00975698086212377f, 0.0129700040415945f, 0.0164423617746781f, 0.0201139756195018f, 0.0239158044840333f, 0.0277716918543809f, 0.0316004950372129f, 0.0353184269141432f, 0.0388415321863768f, 0.0420882143723709f, 0.0449817271456541f, 0.0474525440849641f, 0.0494405245547295f, 0.0508968001199147f, 0.0517853153879127f, 0.0520839691162109f, 0.0517853153879127f, 0.0508968001199147f, 0.0494405245547295f, 0.0474525440849641f, 0.0449817271456541f, 0.0420882143723709f, 0.0388415321863768f, 0.0353184269141432f, 0.0316004950372129f, 0.0277716918543809f, 0.0239158044840333f, 0.0201139756195018f, 0.0164423617746781f, 0.0129700040415945f, 0.00975698086212377f, 0.00685290133382793f, 0.00429578455840068f, 0.00211135600758337f, 0.000312776387504284f, -0.00109919737827612f, -0.00213563505184364f, -0.00281847067702838f, -0.00317919910449468f, -0.00325728724657753f, -0.00309836514905645f, -0.00275226865376297f, -0.00227100929579603f, -0.00170674802675828f, -0.00110984739000886f, -0.000527072017573502f, -1.17681181491811e-018f};

    float avg = 0;
    for (unsigned i = 0; i < NIN_AUDIO_SAMPLE_SIZE; ++i)
    {
        float acc = 0;

        for (unsigned j = 0; j < kLowPassFilterWidth; ++j)
        {
            acc += _samplesRaw[i * kOversampling + j] * kImpulse[j];
        }
        avg += acc;
        _samples[i] = acc;
    }
    avg /= NIN_AUDIO_SAMPLE_SIZE;
    for (unsigned i = 0; i < NIN_AUDIO_SAMPLE_SIZE; ++i)
    {
        _samples[i] -= avg;
    }
}
