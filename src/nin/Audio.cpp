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

void Audio::pushSamples(const float* samples)
{
    ALuint buffer;
    ALint attr;

    /* Unqueue previous buffers */
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &attr);
    while (attr--)
    {
        alSourceUnqueueBuffers(_source, 1, &buffer);
        _buffers.push_back(buffer);
    }

    if (_buffers.empty())
    {
        printf("AUDIO FULL\n");
        fflush(stdout);
        return;
    }

    buffer = _buffers.back();
    _buffers.pop_back();

    alBufferData(buffer, AL_FORMAT_MONO_FLOAT32, samples, NIN_AUDIO_SAMPLE_SIZE * sizeof(*samples), _frequency);
    alSourceQueueBuffers(_source, 1, &buffer);
    alGetSourcei(_source, AL_SOURCE_STATE, &attr);
    if (attr != AL_PLAYING)
    {
        printf("AUDIO EMPTY\n");
        fflush(stdout);
        alSourcePlay(_source);
    }
}
