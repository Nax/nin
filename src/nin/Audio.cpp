#include <nin/nin.h>
#include "Audio.h"

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

#define TOLERANCE (0.0001)

void Audio::pushSamples(const float* samples)
{
    float tmp[NIN_AUDIO_SAMPLE_SIZE + 1];
    int freqDt;
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

    adjustDrift(attr);

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

void Audio::adjustDrift(ALint processedBufferCount)
{
    ALint currentBufferOffset;
    uint32_t queuedSamples;
    double newDrift;

    alGetSourcei(_source, AL_SAMPLE_OFFSET, &currentBufferOffset);
    queuedSamples = NIN_AUDIO_SAMPLE_SIZE * (kBufferCount - _buffers.size()) - currentBufferOffset;
    newDrift = (double)queuedSamples / (double)((kBufferCount / 2) * NIN_AUDIO_SAMPLE_SIZE);
    _playbackDrift = lerp(_playbackDrift, newDrift, 1e-5);
}
