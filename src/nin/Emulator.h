#ifndef EMULATOR_H
#define EMULATOR_H

#include <al.h>
#include <alc.h>

#include <thread>
#include <QString>
#include <nin/nin.h>
#include "MainWindow.h"
#include "Audio.h"

class Emulator : public QObject
{
    Q_OBJECT;

public:
    Emulator();
    virtual ~Emulator();

    void loadRom(const QString& path);
    void closeRom();
    void start();
    void handleInput(uint8_t key, int pressed);
    void handleAudio(const int16_t* samples);
    void update();
    bool workerRunning();

public slots:
    void pause();
    void resume();

private:
    bool            _running;
    bool            _workerRunning;
    std::thread     _worker;
    Audio*          _audio;
    uint8_t         _input;
    NinState*       _state;
    MainWindow*     _window;
};

#endif