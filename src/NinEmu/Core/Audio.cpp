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

#include <nin/nin.h>
#include <NinEmu/Core/Audio.h>

#define TOLERANCE (0.0001)

Audio::Audio(QObject* parent)
: QObject(parent)
{
    ALCint nativeFrequency;

    _device = alcOpenDevice(nullptr);
    _context = alcCreateContext(_device, nullptr);
    alcMakeContextCurrent(_context);

    alcGetIntegerv(_device, ALC_FREQUENCY, 1, &nativeFrequency);
    _frequency = nativeFrequency;

    _buffers.resize(kBufferCount);
    alGenBuffers(kBufferCount, _buffers.data());
    alGenSources(1, &_source);
}

Audio::~Audio()
{
    alDeleteSources(1, &_source);
    alDeleteBuffers(kBufferCount, _buffers.data());
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

uint32_t Audio::frequency() const
{
    return _frequency;
}

void Audio::reset()
{
    static const float kSilence[NIN_AUDIO_SAMPLE_SIZE] = { 0.f };

    ALint attr;
    ALuint buffer;

    alSourceStop(_source);
    alGetSourcei(_source, AL_BUFFERS_QUEUED, &attr);
    for (int i = 0; i < attr; ++i)
    {
        alSourceUnqueueBuffers(_source, 1, &buffer);
        _buffers.push_back(buffer);
    }

    for (int i = 0; i < kBufferCount / 2; ++i)
    {
        buffer = _buffers.back();
        _buffers.pop_back();
        alBufferData(buffer, AL_FORMAT_MONO_FLOAT32, kSilence, NIN_AUDIO_SAMPLE_SIZE * sizeof(*kSilence), 48000);
        alSourceQueueBuffers(_source, 1, &buffer);
    }

    _playbackDrift = 1.0;
    alSourcePlay(_source);
}

void Audio::pushSamples(const float* samples)
{
    float tmp[NIN_AUDIO_SAMPLE_SIZE + 1];
    ALuint buffer;
    ALint attr;
    ALsizei len;

    len = NIN_AUDIO_SAMPLE_SIZE;

    /* Alloc an extra, duplicate samples */
    memcpy(tmp, samples, NIN_AUDIO_SAMPLE_SIZE * sizeof(*samples));
    tmp[NIN_AUDIO_SAMPLE_SIZE] = tmp[NIN_AUDIO_SAMPLE_SIZE - 1];

    if (_playbackDrift < 1.0 - TOLERANCE)
    {
        _playbackDrift = 1.0;
        len++;
    }
    else if (_playbackDrift > 1.0 + TOLERANCE)
    {
        _playbackDrift = 1.0;
        len--;
    }

    /* Unqueue previous buffers */
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &attr);
    for (int i = 0; i < attr; ++i)
    {
        alSourceUnqueueBuffers(_source, 1, &buffer);
        _buffers.push_back(buffer);
    }

    adjustDrift();

    if (_buffers.empty())
    {
        printf("AUDIO FULL\n");
        fflush(stdout);
        return;
    }

    buffer = _buffers.back();
    _buffers.pop_back();

    alBufferData(buffer, AL_FORMAT_MONO_FLOAT32, tmp, len * sizeof(*tmp), 48000);
    alSourceQueueBuffers(_source, 1, &buffer);
    alGetSourcei(_source, AL_SOURCE_STATE, &attr);
    if (attr != AL_PLAYING)
    {
        printf("AUDIO EMPTY\n");
        fflush(stdout);
        alSourcePlay(_source);
    }
}

static double lerp(double a, double b, double coeff)
{
    return (1.f - coeff) * a + coeff * b;
}

void Audio::adjustDrift()
{
    ALint currentBufferOffset;
    std::size_t queuedSamples;
    double newDrift;

    alGetSourcei(_source, AL_SAMPLE_OFFSET, &currentBufferOffset);
    queuedSamples = NIN_AUDIO_SAMPLE_SIZE * (kBufferCount - _buffers.size()) - currentBufferOffset;
    newDrift = (double)queuedSamples / (double)((kBufferCount / 2) * NIN_AUDIO_SAMPLE_SIZE);
    _playbackDrift = lerp(_playbackDrift, newDrift, 1e-5);
}
