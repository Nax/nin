#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>

#include <cstdint>
#include <vector>
#include <al.h>
#include <alext.h>
#include <alc.h>

class Audio : public QObject
{
    Q_OBJECT;

public:
    explicit Audio(QObject* parent = nullptr);
    ~Audio();

public slots:
    void pushSamples(const float* samples);

private:
    static const unsigned kBufferCount = 4;

    ALCdevice*          _device;
    ALCcontext*         _context;
    ALuint              _source;
    std::vector<ALuint> _buffers;
};

#endif
