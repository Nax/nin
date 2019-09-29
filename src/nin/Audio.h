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
    virtual ~Audio();

    uint32_t frequency() const;

public slots:
    void reset();
    void pushSamples(const float* samples);

private:
    static const unsigned kBufferCount = 4;

    void adjustDrift(int numBuf);

    ALCdevice*          _device;
    ALCcontext*         _context;
    ALuint              _source;
    std::vector<ALuint> _buffers;
    uint32_t            _frequency;
    double              _playbackDrift;
};

#endif
