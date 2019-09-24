#include <QKeyEvent>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QByteArray>
#include <QLayout>
#include <QApplication>

#include <nin/nin.h>

#include "MainWindow.h"
#include "EmulatorWorker.h"
#include "Audio.h"
#include "RenderWidget.h"
#include "MemoryWindow.h"

#define DEADZONE (0.30)

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
    static const int scale = 3;
    int dx;
    int dy;

    _emu = new EmulatorWorker(this);
    _audio = new Audio(this);

    setWindowTitle("Nin " NIN_VERSION);

    _render = new RenderWidget();
    setCentralWidget(_render);
    _render->show();

    createActions();
    createMenus();

    layout()->update();
    layout()->activate();
    dx = size().width() - _render->size().width();
    dy = size().height() - _render->size().height();
    resize(256 * scale + dx, 240 * scale + dy);

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

void MainWindow::updateTexture(const char* texture)
{
    _render->updateTexture(texture);
    //_render->update();
}

void MainWindow::openRom()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("&Open ROM"), "", "*.nes");
    if (!filename.isEmpty())
    {
        QByteArray rawFilename = filename.toUtf8();
        _emu->loadRom(rawFilename.data());
    }
}

void MainWindow::openMemoryViewer()
{
    MemoryWindow* win;

    win = new MemoryWindow();
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
    win->activateWindow();
}

void MainWindow::createActions()
{
    _openRom = new QAction(tr("&Open ROM"), this);
    _openRom->setShortcut(QKeySequence::Open);
    connect(_openRom, &QAction::triggered, this, &MainWindow::openRom);

    _pauseEmulation = new QAction(tr("&Pause"), this);
    connect(_pauseEmulation, &QAction::triggered, _emu, &EmulatorWorker::pause);

    _resumeEmulation = new QAction(tr("&Resume"), this);
    connect(_resumeEmulation, &QAction::triggered, _emu, &EmulatorWorker::resume);

    _actionMemoryViewer = new QAction(tr("&Memory Viewer"), this);
    connect(_actionMemoryViewer, &QAction::triggered, this, &MainWindow::openMemoryViewer);
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_openRom);

    _emulationMenu = menuBar()->addMenu(tr("&Emulation"));
    _emulationMenu->addAction(_pauseEmulation);
    _emulationMenu->addAction(_resumeEmulation);

    _windowMenu = menuBar()->addMenu(tr("&Window"));
    _windowMenu->addAction(_actionMemoryViewer);
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
