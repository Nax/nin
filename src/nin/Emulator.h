#ifndef EMULATOR_H
#define EMULATOR_H

#include <al.h>
#include <alc.h>

#include <thread>
#include <QTimer>
#include <nin/nin.h>
#include "MainWindow.h"
#include "Audio.h"

class Emulator : public QObject
{
    Q_OBJECT;

public:
    Emulator();
    void loadRom(const char* path);
    void start();
    void handleInput(uint8_t key, int pressed);
    void handleAudio(const int16_t* samples);
    void update();

public slots:
    void pause();
    void resume();

private:
    bool            _running;
    std::thread     _worker;
    Audio*          _audio;
    uint8_t         _input;
    NinState*       _state;
    MainWindow*     _window;
};

#endif
