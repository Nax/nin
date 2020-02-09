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
#include <libnin/Audio.h>
#include <libnin/HardwareInfo.h>

using namespace libnin;

Audio::Audio(const HardwareInfo& info)
: _info(info)
, _callback(nullptr)
, _callbackArg(nullptr)
, _targetFrequency(0)
, _accumulator(0)
, _samplesCursor(0)
, _loPassX{}
, _loPassY{}
, _hiPassX{}
, _hiPassY{}
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
    sample = loPass(sample);
    _accumulator += _targetFrequency;
    if (_accumulator >= _info.specs().clockRate)
    {
        sample = hiPass(sample);
        _accumulator -= _info.specs().clockRate;
        _samples[_samplesCursor++] = sample;
        if (_samplesCursor >= NIN_AUDIO_SAMPLE_SIZE)
        {
            _samplesCursor = 0;
            if (_callback)
                _callback(_callbackArg, _samples);
        }
    }
}

double Audio::loPass(double sample)
{
    static constexpr const double kGain = 6.259598570e+07;

    _loPassX[0] = _loPassX[1];
    _loPassX[1] = _loPassX[2];
    _loPassX[2] = _loPassX[3];
    _loPassX[3] = _loPassX[4];
    _loPassX[4] = _loPassX[5];

    _loPassX[5] = sample / kGain;

    _loPassY[0] = _loPassY[1];
    _loPassY[1] = _loPassY[2];
    _loPassY[2] = _loPassY[3];
    _loPassY[3] = _loPassY[4];
    _loPassY[4] = _loPassY[5];

    _loPassY[5] = (
        (_loPassX[0] + _loPassX[5])
        + 5 * (_loPassX[1] + _loPassX[4])
        + 10 * (_loPassX[2] + _loPassX[3])
        + 0.8337693903 * _loPassY[0]
        - 4.3203944730 * _loPassY[1]
        + 8.9577480484 * _loPassY[2]
        - 9.2893615611 * _loPassY[3]
        + 4.8182380843 * _loPassY[4]
        );

    return _loPassY[5];
}

double Audio::hiPass(double sample)
{
    static constexpr const double kGain = 1.000654713;

    _hiPassX[0] = _hiPassX[1];
    _hiPassX[1] = _hiPassX[2];
    _hiPassX[2] = _hiPassX[3];

    _hiPassX[3] = sample / kGain;

    _hiPassY[0] = _hiPassY[1];
    _hiPassY[1] = _hiPassY[2];
    _hiPassY[2] = _hiPassY[3];

    _hiPassY[3] = (
        (_hiPassX[3] - _hiPassX[0])
        + 3 * (_hiPassX[1] - _hiPassX[2])
        + 0.9986918594 * _hiPassY[0]
        - 2.9973828628 * _hiPassY[1]
        + 2.9986910031 * _hiPassY[2]
        );

    return _hiPassY[3];
}
