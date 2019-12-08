/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QtWidgets>
#include <nin/nin.h>

#include <NinEmu/Core/Audio.h>
#include <NinEmu/Core/EmulatorWorker.h>
#include <NinEmu/Core/Settings.h>
#include <NinEmu/Menu/GraphicsMenu.h>
#include <NinEmu/UI/RenderWidget.h>
#include <NinEmu/Window/AudioVisualizerWindow.h>
#include <NinEmu/Window/DebuggerWindow.h>
#include <NinEmu/Window/MainWindow.h>
#include <NinEmu/Window/MemoryWindow.h>

#define DEADZONE (0.30)

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
    static const int scale = 3;
    int dx;
    int dy;

    _settings = new Settings(this);
    _audio = new Audio(this);
    _emu = new EmulatorWorker(this);
    _emu->setAudioFrequency(48000);

    setWindowTitle("Nin " NIN_VERSION);

    _render = new RenderWidget();
    setCentralWidget(_render);
    _render->show();

    createActions();
    createMenus();
    updateRecentFiles();

    layout()->update();
    layout()->activate();
    dx = size().width() - _render->size().width();
    dy = size().height() - _render->size().height();
    resize(256 * scale + dx, 240 * scale + dy);

    connect(_emu, &EmulatorWorker::reset, _audio, &Audio::reset, Qt::DirectConnection);
    connect(_emu, &EmulatorWorker::audio, _audio, &Audio::pushSamples, Qt::DirectConnection);
    connect(_emu, &EmulatorWorker::frame, _render, &RenderWidget::updateTexture, Qt::DirectConnection);

    setupGamepad();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Z:
        _emu->inputKeyPress(NIN_BUTTON_A);
        break;
    case Qt::Key_X:
        _emu->inputKeyPress(NIN_BUTTON_B);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        _emu->inputKeyPress(NIN_BUTTON_START);
        break;
    case Qt::Key_Space:
        _emu->inputKeyPress(NIN_BUTTON_SELECT);
        break;
    case Qt::Key_Left:
        _emu->inputKeyPress(NIN_BUTTON_LEFT);
        break;
    case Qt::Key_Right:
        _emu->inputKeyPress(NIN_BUTTON_RIGHT);
        break;
    case Qt::Key_Up:
        _emu->inputKeyPress(NIN_BUTTON_UP);
        break;
    case Qt::Key_Down:
        _emu->inputKeyPress(NIN_BUTTON_DOWN);
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Z:
        _emu->inputKeyRelease(NIN_BUTTON_A);
        break;
    case Qt::Key_X:
        _emu->inputKeyRelease(NIN_BUTTON_B);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        _emu->inputKeyRelease(NIN_BUTTON_START);
        break;
    case Qt::Key_Space:
        _emu->inputKeyRelease(NIN_BUTTON_SELECT);
        break;
    case Qt::Key_Left:
        _emu->inputKeyRelease(NIN_BUTTON_LEFT);
        break;
    case Qt::Key_Right:
        _emu->inputKeyRelease(NIN_BUTTON_RIGHT);
        break;
    case Qt::Key_Up:
        _emu->inputKeyRelease(NIN_BUTTON_UP);
        break;
    case Qt::Key_Down:
        _emu->inputKeyRelease(NIN_BUTTON_DOWN);
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!_windowMemoryViewer.isNull())
        _windowMemoryViewer->close();
    if (!_windowAudioVisualizer.isNull())
        _windowAudioVisualizer->close();
    if (!_windowDebugger.isNull())
        _windowDebugger->close();
    QMainWindow::closeEvent(event);
}

void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open ROM"), "", "NES/FDS rom (*.nes *.fds)");
    openRom(filename);
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        openRom(action->data().toString());
}

void MainWindow::openWindowMemoryViewer()
{
    MemoryWindow* win;

    if (_windowMemoryViewer.isNull())
    {
        win = new MemoryWindow;
        win->setAttribute(Qt::WA_DeleteOnClose);
        connect(_emu, SIGNAL(update(NinState*)), win, SLOT(refresh(NinState*)));
        win->show();
        _windowMemoryViewer = win;
    }
    else
        _windowMemoryViewer->activateWindow();
}

void MainWindow::openWindowAudioVisualizer()
{
    AudioVisualizerWindow* win;

    if (_windowAudioVisualizer.isNull())
    {
        win = new AudioVisualizerWindow;
        win->setAttribute(Qt::WA_DeleteOnClose);
        connect(_emu, &EmulatorWorker::audio, win, &AudioVisualizerWindow::refresh, Qt::DirectConnection);
        win->show();
        _windowAudioVisualizer = win;
    }
    else
        _windowAudioVisualizer->activateWindow();
}

void MainWindow::openWindowDebugger()
{
    DebuggerWindow* win;

    if (_windowDebugger.isNull())
    {
        win = new DebuggerWindow;
        win->setAttribute(Qt::WA_DeleteOnClose);
        connect(_emu, SIGNAL(update(NinState*)), win, SLOT(refresh(NinState*)));
        win->show();
        _windowDebugger = win;
    }
    else
        _windowDebugger->activateWindow();
}

void MainWindow::createActions()
{
    _actionOpenFile = new QAction(tr("&Open..."), this);
    _actionOpenFile->setShortcut(QKeySequence::Open);
    connect(_actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);

    for (size_t i = 0; i < MaxRecentFiles; ++i)
    {
        _actionOpenRecentFile[i] = new QAction(this);
        _actionOpenRecentFile[i]->setVisible(false);
        connect(_actionOpenRecentFile[i], &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    _actionExit = new QAction(tr("E&xit"), this);
    _actionExit->setShortcut(QKeySequence::Quit);
    connect(_actionExit, &QAction::triggered, this, &QWidget::close);

    _pauseEmulation = new QAction(tr("Pause"), this);
    connect(_pauseEmulation, &QAction::triggered, _emu, &EmulatorWorker::pause);

    _resumeEmulation = new QAction(tr("Resume"), this);
    connect(_resumeEmulation, &QAction::triggered, _emu, &EmulatorWorker::resume);

    _actionStepFrame = new QAction(tr("Step frame"), this);
    connect(_actionStepFrame, &QAction::triggered, _emu, &EmulatorWorker::stepFrame);

    _actionStepSingle = new QAction(tr("Step single"), this);
    connect(_actionStepSingle, &QAction::triggered, _emu, &EmulatorWorker::stepSingle);

    _actionMemoryViewer = new QAction(tr("Memory Viewer"), this);
    connect(_actionMemoryViewer, &QAction::triggered, this, &MainWindow::openWindowMemoryViewer);

    _actionAudioVisualizer = new QAction(tr("Audio Visualizer"), this);
    connect(_actionAudioVisualizer, &QAction::triggered, this, &MainWindow::openWindowAudioVisualizer);

    _actionDebugger = new QAction(tr("Debugger"), this);
    connect(_actionDebugger, &QAction::triggered, this, &MainWindow::openWindowDebugger);
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_actionOpenFile);
    _fileMenu->addSeparator();
    for (size_t i = 0; i < MaxRecentFiles; ++i)
        _fileMenu->addAction(_actionOpenRecentFile[i]);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_actionExit);

    _emulationMenu = menuBar()->addMenu(tr("&Emulation"));
    _emulationMenu->addAction(_pauseEmulation);
    _emulationMenu->addAction(_resumeEmulation);
    _emulationMenu->addAction(_actionStepFrame);
    _emulationMenu->addAction(_actionStepSingle);

    _windowMenu = menuBar()->addMenu(tr("&Window"));
    _windowMenu->addAction(_actionMemoryViewer);
    _windowMenu->addAction(_actionAudioVisualizer);
    _windowMenu->addAction(_actionDebugger);

    _graphicsMenu = new GraphicsMenu(this);
    connect(_graphicsMenu, &GraphicsMenu::fullscreen, this, &MainWindow::eventFullscreen);
    connect(_graphicsMenu, &GraphicsMenu::fit, this, &MainWindow::eventFit);
    connect(_graphicsMenu, &GraphicsMenu::integerScale, this, &MainWindow::eventIntegerScale);
    connect(_graphicsMenu, &GraphicsMenu::aspectRatio, this, &MainWindow::eventAspectRatio);
    connect(_graphicsMenu, &GraphicsMenu::overscan, this, &MainWindow::eventOverscan);
    menuBar()->addMenu(_graphicsMenu);
}

void MainWindow::updateRecentFiles()
{
    QSettings settings;
    QStringList files;
    int count;

    files = settings.value("recentFiles").toStringList();
    count = qMin((int)files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < count; ++i) {
        QString text = tr("&%1. %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        _actionOpenRecentFile[i]->setText(text);
        _actionOpenRecentFile[i]->setData(files[i]);
        _actionOpenRecentFile[i]->setVisible(true);
    }
}

void MainWindow::addToRecentFiles(const QString& filename)
{
    QSettings settings;
    QStringList files;

    files = settings.value("recentFiles").toStringList();
    files.removeAll(filename);
    files.prepend(filename);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFiles", files);
    updateRecentFiles();
}

void MainWindow::setupGamepad()
{
    QList<int> gamepads;
    for (int i = 0; i < 10000; ++i) {
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
            _emu->inputKeyPress(NIN_BUTTON_LEFT);
            _emu->inputKeyRelease(NIN_BUTTON_RIGHT);
        }
        else if (value > DEADZONE)
        {
            _emu->inputKeyRelease(NIN_BUTTON_LEFT);
            _emu->inputKeyPress(NIN_BUTTON_RIGHT);
        }
        else
        {
            _emu->inputKeyRelease(NIN_BUTTON_LEFT);
            _emu->inputKeyRelease(NIN_BUTTON_RIGHT);
        }
    });

    connect(_gamepad, &QGamepad::axisLeftYChanged, this, [this](double value) {
        if (value < -DEADZONE)
        {
            _emu->inputKeyRelease(NIN_BUTTON_DOWN);
            _emu->inputKeyPress(NIN_BUTTON_UP);
        }
        else if (value > DEADZONE)
        {
            _emu->inputKeyPress(NIN_BUTTON_DOWN);
            _emu->inputKeyRelease(NIN_BUTTON_UP);
        }
        else
        {
            _emu->inputKeyRelease(NIN_BUTTON_DOWN);
            _emu->inputKeyRelease(NIN_BUTTON_UP);
        }
    });

    connect(_gamepad, &QGamepad::buttonAChanged, this, [this](bool pressed) {
        if (pressed)
            _emu->inputKeyPress(NIN_BUTTON_A);
        else
            _emu->inputKeyRelease(NIN_BUTTON_A);
    });
    connect(_gamepad, &QGamepad::buttonXChanged, this, [this](bool pressed) {
        if (pressed)
            _emu->inputKeyPress(NIN_BUTTON_B);
        else
            _emu->inputKeyRelease(NIN_BUTTON_B);
    });
    connect(_gamepad, &QGamepad::buttonStartChanged, this, [this](bool pressed) {
        if (pressed)
            _emu->inputKeyPress(NIN_BUTTON_START);
        else
            _emu->inputKeyRelease(NIN_BUTTON_START);
    });
    connect(_gamepad, &QGamepad::buttonSelectChanged, this, [this](bool pressed) {
        if (pressed)
            _emu->inputKeyPress(NIN_BUTTON_SELECT);
        else
            _emu->inputKeyRelease(NIN_BUTTON_SELECT);
    });
}

void MainWindow::openRom(const QString& filename)
{
    if (!filename.isEmpty())
    {
        QByteArray rawFilename = filename.toUtf8();
        if (_emu->loadRom(rawFilename.data()))
            addToRecentFiles(filename);
    }
}

void MainWindow::eventFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        showFullScreen();
#if defined(WIN32) || defined(_WIN32)
        menuBar()->hide();
#endif
    }
    else
    {
        showNormal();
        menuBar()->show();
    }
}

void MainWindow::eventFit(bool fit)
{
    _render->setFit(fit);
}

void MainWindow::eventIntegerScale(bool integerScale)
{
    _render->setIntegerScale(integerScale);
}

void MainWindow::eventAspectRatio(AspectRatio aspectRatio)
{
    switch (aspectRatio)
    {
    case AspectRatio::Square:
        _render->setPixelAspectRatio(1.0f);
        break;
    case AspectRatio::NTSC:
        _render->setPixelAspectRatio(8.f / 7.f);
        break;
    }
}

void MainWindow::eventOverscan(Overscan overscan)
{
    switch (overscan)
    {
    case Overscan::None:
        _render->setOverscan(0, 0);
        break;
    case Overscan::Small:
        _render->setOverscan(0, 8);
        break;
    case Overscan::Medium:
        _render->setOverscan(8, 16);
        break;
    case Overscan::Large:
        _render->setOverscan(16, 24);
        break;
    }
}
