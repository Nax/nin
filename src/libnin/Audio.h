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

#ifndef LIBNIN_AUDIO_H
#define LIBNIN_AUDIO_H 1

#include <cstdint>
#include <nin/nin.h>
#include <libnin/NonCopyable.h>

class Audio : private NonCopyable
{
public:
    Audio();
    ~Audio();

    void setCallback(NINAUDIOCALLBACK callback, void* arg);
    void setSourceFrequency(std::uint32_t freq);
    void setTargetFrequency(std::uint32_t freq);

    void push(float sample);

private:
    double loPass(double sample);
    double hiPass(double sample);

    NINAUDIOCALLBACK    _callback;
    void*               _callbackArg;
    std::uint32_t       _sourceFrequency;
    std::uint32_t       _targetFrequency;
    std::uint32_t       _accumulator;
    float               _samples[NIN_AUDIO_SAMPLE_SIZE];
    std::uint16_t       _samplesCursor;
    double              _loPassX[6];
    double              _loPassY[6];
    double              _hiPassX[4];
    double              _hiPassY[4];
};

#endif
