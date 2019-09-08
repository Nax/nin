#include <nin/nin.h>
#include "Audio.h"

Audio::Audio()
{
    _device = alcOpenDevice(nullptr);
    _context = alcCreateContext(_device, nullptr);
    alcMakeContextCurrent(_context);
    _buffers.resize(kBufferCount);
    alGenBuffers(kBufferCount, _buffers.data());
    alGenSources(1, &_source);
}

Audio::~Audio()
{

}

void Audio::pushSamples(const int16_t* samples)
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

    alBufferData(buffer, AL_FORMAT_MONO16, samples, NIN_AUDIO_SAMPLE_SIZE * 2, 48000);
    alSourceQueueBuffers(_source, 1, &buffer);
    alGetSourcei(_source, AL_SOURCE_STATE, &attr);
    if (attr != AL_PLAYING)
    {
        printf("AUDIO EMPTY\n");
        fflush(stdout);
        alSourcePlay(_source);
    }
}
