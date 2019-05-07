#ifndef NIN_EMULATOR_H
#define NIN_EMULATOR_H

#include <al.h>
#include <alc.h>

#include <QTimer>
#include <nin/nin.h>
#include "NinMainWindow.h"
#include "Audio.h"

class NinEmulator : public QObject
{
    Q_OBJECT;

public:
    NinEmulator(const char* path);
    void start();
    void handleInput(uint8_t key, int pressed);
    void handleAudio(const int16_t* samples);

private slots:
    void update();

private:
    Audio*          _audio;
    uint8_t         _input;
    QTimer*         _timer;
    NinState*       _state;
    NinMainWindow*  _window;
};

#endif
