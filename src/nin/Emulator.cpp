#include <cstdio>
#include <chrono>
#include <thread>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include "Emulator.h"

static void _audioCallback(void* arg, const int16_t* samples)
{
    ((Emulator*)arg)->handleAudio(samples);
}

static void _workerMain(Emulator* emu)
{
    static const uint64_t kDelay = 16639489 / 4;

    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::nanoseconds;
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    TimePoint prev;
    TimePoint now;
    uint64_t acc;

    prev = Clock::now();
    acc = 0;
    for (;;)
    {
        if (!emu->workerRunning())
            return;

        now = Clock::now();
        acc += (now - prev).count();
        prev = now;

        while (acc >= kDelay)
        {
            QMetaObject::invokeMethod(emu, &Emulator::update);
            acc -= kDelay;
        }

        std::this_thread::sleep_for(std::chrono::nanoseconds(kDelay - acc));
    }
}

Emulator::Emulator()
: QObject(nullptr)
, _running(false)
, _workerRunning(false)
, _state(nullptr)
{
    _audio = new Audio;
    _window = new MainWindow(*this);
    _window->setAttribute(Qt::WA_DeleteOnClose);
    _window->show();

    _input = 0;
}

Emulator::~Emulator()
{
    closeRom();
    _workerRunning = false;
    _worker.join();
    delete _audio;
}

void Emulator::closeRom()
{
    if (_state)
    {
        ninDestroyState(_state);
        _state = nullptr;
    }
}

void Emulator::loadRom(const QString& path)
{
    QByteArray raw;
    QString saveFile;

    closeRom();
    raw = path.toUtf8();
    _state = ninCreateState(raw.data());
    if (_state)
    {
        QFileInfo info(path);
        saveFile = info.path() + "/" + info.completeBaseName() + ".sav";
        raw = saveFile.toUtf8();
        ninSetSaveFile(_state, raw.data());
        ninSetAudioCallback(_state, &_audioCallback, this);
    }
}

void Emulator::exit()
{
    QApplication::quit();
}

void Emulator::start()
{
    // Run the first frame
    _running = true;
    update();
    _workerRunning = true;
    _worker = std::thread(_workerMain, this);
}

void Emulator::handleInput(uint8_t key, int pressed)
{
    if (pressed)
        _input |= key;
    else
        _input &= ~key;
}

void Emulator::handleAudio(const int16_t* samples)
{
    _audio->pushSamples(samples);
}

void Emulator::update()
{
    if (!_state || !_running)
        return;

    ninSetInput(_state, _input);
    if (ninRunCycles(_state, 29781 / 4))
        _window->updateTexture((const char*)ninGetScreenBuffer(_state));
    emit gameUpdate(_state);
}

bool Emulator::workerRunning()
{
    return _workerRunning;
}

void Emulator::pause()
{
    _running = false;
}

void Emulator::resume()
{
    _running = true;
}
