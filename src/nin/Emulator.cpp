#include <stdio.h>
#include <QFileInfo>
#include <QApplication>
#include "Emulator.h"
#include "MainWindow.h"
#include "Audio.h"

#define DEADZONE (0.30)

Emulator::Emulator()
: _state(nullptr)
, _workerState(WorkerState::Idle)
, _input(0)
{
    _thread = std::thread(&Emulator::workerMain, this);

    _mainWindow = new MainWindow(*this);
    _mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    _mainWindow->show();

    _audio = new Audio;

    setupGamepad();
}

Emulator::~Emulator()
{
    std::unique_lock lock(_mutex);

    _workerState = WorkerState::Stopping;
    lock.unlock();
    _cv.notify_one();

    _thread.join();
}

void Emulator::loadRom(const QString& path)
{
    QByteArray raw;
    QString saveFile;
    NinError err;
    NinInt32 frameCycles;
    NinInt32 frameDelay;

    std::unique_lock lock(_mutex);

    closeRomRaw();
    raw = path.toUtf8();
    err = ninCreateState(&_state, raw.data());
    if (err != NIN_OK)
    {
        printf("Error loading rom (%d)\n", (int)err);
        fflush(stdout);
        _workerState = WorkerState::Idle;
    }
    else
    {
        ninInfoQueryInteger(_state, &frameCycles, NIN_INFO_FRAME_CYCLES);
        ninInfoQueryInteger(_state, &frameDelay, NIN_INFO_FRAME_DELAY);

        _frameCycles = frameCycles;
        _frameDelay = frameDelay;
        _cyc = 0;
        _accumulator = 0;

        QFileInfo info(path);
        saveFile = info.path() + "/" + info.completeBaseName() + ".sav";
        raw = saveFile.toUtf8();
        ninSetSaveFile(_state, raw.data());
        ninAudioSetCallback(_state, (NINAUDIOCALLBACK)&audioCallback, this);
        _workerState = WorkerState::Starting;
    }

    lock.unlock();
    _cv.notify_one();
}

void Emulator::closeRom()
{
    std::unique_lock lock(_mutex);

    closeRomRaw();
    _workerState = WorkerState::Idle;

    lock.unlock();
    _cv.notify_one();
}

void Emulator::pause()
{
    std::unique_lock lock(_mutex);

    if (_workerState == WorkerState::Running)
    {
        _workerState = WorkerState::Paused;
        lock.unlock();
        _cv.notify_one();
    }
}

void Emulator::resume()
{
    std::unique_lock lock(_mutex);

    if (_workerState == WorkerState::Paused)
    {
        _workerState = WorkerState::Starting;
        lock.unlock();
        _cv.notify_one();
    }
}

void Emulator::inputKeyPress(uint8_t key)
{
    _input |= key;
}

void Emulator::inputKeyRelease(uint8_t key)
{
    _input &= ~key;
}

void Emulator::workerMain()
{
    TimePoint prev;
    TimePoint now;
    uint64_t dt;
    uint64_t delay;

    std::unique_lock lock(_mutex);

    prev = Clock::now();
    _accumulator = 0;

    for (;;)
    {
        now = Clock::now();
        dt = (now - prev).count();
        prev = now;

        switch (_workerState)
        {
        case WorkerState::Idle:
        case WorkerState::Paused:
            _cv.wait(lock);
            break;
        case WorkerState::Starting:
            workerUpdate();
            _accumulator = 0;
            now = Clock::now();
            prev = now;
            _workerState = WorkerState::Running;
            break;
        case WorkerState::Running:
            _accumulator += dt;
            delay = _frameDelay / 4;
            while (_accumulator >= delay)
            {
                _accumulator -= delay;
                workerUpdate();
            }
            _cv.wait_for(lock, std::chrono::nanoseconds(delay - _accumulator));
            break;
        case WorkerState::Stopping:
            return;
        }
    }
}

void Emulator::workerUpdate()
{
    size_t cyc;

    cyc = _cyc;
    ninSetInput(_state, _input);
    if (ninRunCycles(_state, _frameCycles / 4 - cyc, &cyc))
    {
        _mainWindow->updateTexture((const char*)ninGetScreenBuffer(_state));
    }
    _cyc = cyc;
    //emit gameUpdate(_state);
}

void Emulator::closeRomRaw()
{
    if (_state)
    {
        ninDestroyState(_state);
        _state = nullptr;
    }
}

void Emulator::audioCallback(Emulator* emu, const int16_t* samples)
{
    emu->_audio->pushSamples(samples);
}

void Emulator::setupGamepad()
{
    QList<int> gamepads;
    for (int i = 0; i < 1000; ++i) {
        QApplication::processEvents();
        gamepads = QGamepadManager::instance()->connectedGamepads();
        if (!gamepads.isEmpty())
            break;
    }
    if (gamepads.isEmpty()) {
        printf("No gamepad detected.\n");
        fflush(stdout);
        return;
    }

    _gamepad = new QGamepad(*gamepads.begin(), this);

    connect(_gamepad, &QGamepad::axisLeftXChanged, this, [this](double value) {
        if (value < -DEADZONE)
        {
            inputKeyPress(NIN_BUTTON_LEFT);
            inputKeyRelease(NIN_BUTTON_RIGHT);
        }
        else if (value > DEADZONE)
        {
            inputKeyRelease(NIN_BUTTON_LEFT);
            inputKeyPress(NIN_BUTTON_RIGHT);
        }
        else
        {
            inputKeyRelease(NIN_BUTTON_LEFT);
            inputKeyRelease(NIN_BUTTON_RIGHT);
        }
    });

    connect(_gamepad, &QGamepad::axisLeftYChanged, this, [this](double value) {
        if (value < -DEADZONE)
        {
            inputKeyRelease(NIN_BUTTON_DOWN);
            inputKeyPress(NIN_BUTTON_UP);
        }
        else if (value > DEADZONE)
        {
            inputKeyPress(NIN_BUTTON_DOWN);
            inputKeyRelease(NIN_BUTTON_UP);
        }
        else
        {
            inputKeyRelease(NIN_BUTTON_DOWN);
            inputKeyRelease(NIN_BUTTON_UP);
        }
    });

    connect(_gamepad, &QGamepad::buttonAChanged, this, [this](bool pressed) {
        if (pressed)
            inputKeyPress(NIN_BUTTON_A);
        else
            inputKeyRelease(NIN_BUTTON_A);
    });
    connect(_gamepad, &QGamepad::buttonXChanged, this, [this](bool pressed) {
        if (pressed)
            inputKeyPress(NIN_BUTTON_B);
        else
            inputKeyRelease(NIN_BUTTON_B);
    });
    connect(_gamepad, &QGamepad::buttonStartChanged, this, [this](bool pressed) {
        if (pressed)
            inputKeyPress(NIN_BUTTON_START);
        else
            inputKeyRelease(NIN_BUTTON_START);
    });
    connect(_gamepad, &QGamepad::buttonSelectChanged, this, [this](bool pressed) {
        if (pressed)
            inputKeyPress(NIN_BUTTON_SELECT);
        else
            inputKeyRelease(NIN_BUTTON_SELECT);
    });
}
