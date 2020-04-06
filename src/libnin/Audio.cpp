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
, _samplesCursor{kLowPassFilterWidth / 2}
, _samplesRaw{}
{

}

Audio::~Audio()
{

}

void Audio::setCallback(NINAUDIOCALLBACK callback, void* arg)
{
    _callback = callback;
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
        0.f,                    0.f,                    -0.000527072017573503f, -0.00110984739000885f,
        -0.00170674802675828f,  -0.00227100929579603f,  -0.00275226865376297f,  -0.00309836514905645f,
        -0.00325728724657753f,  -0.00317919910449467f,  -0.00281847067702838f,  -0.00213563505184364f,
        -0.00109919737827612f,  0.000312776387504284f,  0.00211135600758337f,   0.00429578455840068f,
        0.00685290133382793f,   0.00975698086212377f,   0.0129700040415945f,    0.0164423617746781f,
        0.0201139756195018f,    0.0239158044840333f,    0.0277716918543809f,    0.0316004950372129f,
        0.0353184269141432f,    0.0388415321863768f,    0.0420882143723709f,    0.0449817271456541f,
        0.0474525440849641f,    0.0494405245547295f,    0.0508968001199147f,    0.0517853153879127f,
        0.0520839691162109f,    0.0517853153879127f,    0.0508968001199147f,    0.0494405245547295f,
        0.0474525440849641f,    0.0449817271456541f,    0.0420882143723709f,    0.0388415321863768f,
        0.0353184269141432f,    0.0316004950372129f,    0.0277716918543809f,    0.0239158044840333f,
        0.0201139756195018f,    0.0164423617746781f,    0.0129700040415945f,    0.00975698086212377f,
        0.00685290133382793f,   0.00429578455840068f,   0.00211135600758337f,   0.000312776387504284f,
        -0.00109919737827612f,  -0.00213563505184364f,  -0.00281847067702838f,  -0.00317919910449468f,
        -0.00325728724657753f,  -0.00309836514905645f,  -0.00275226865376297f,  -0.00227100929579603f,
        -0.00170674802675828f,  -0.00110984739000886f,  -0.000527072017573502f, -1.17681181491811e-018f
    };

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
