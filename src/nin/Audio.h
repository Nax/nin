#ifndef AUDIO_H
#define AUDIO_H

#include <cstdint>
#include <vector>
#include <al.h>
#include <alc.h>

class Audio
{
public:
    Audio();
    ~Audio();

    void pushSamples(const int16_t* samples);

private:
    static const unsigned kBufferCount = 8;

    ALCdevice*          _device;
    ALCcontext*         _context;
    ALuint              _source;
    std::vector<ALuint> _buffers;
};

#endif
