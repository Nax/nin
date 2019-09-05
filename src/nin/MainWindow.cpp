#include <QKeyEvent>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QByteArray>

#include "MainWindow.h"
#include "Emulator.h"
#include "RenderWidget.h"

MainWindow::MainWindow(Emulator& emu, QWidget* parent)
: QMainWindow(parent)
, _emu(emu)
{
    static const int scale = 3;

    resize(256 * scale, 240 * scale);
    setWindowTitle("Nin");


    _render = new RenderWidget(emu);
    setCentralWidget(_render);
    _render->show();

    createActions();
    createMenus();
}

MainWindow::~MainWindow()
{

}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Z:
        _emu.handleInput(NIN_BUTTON_A, 1);
        break;
    case Qt::Key_X:
        _emu.handleInput(NIN_BUTTON_B, 1);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        _emu.handleInput(NIN_BUTTON_START, 1);
        break;
    case Qt::Key_Space:
        _emu.handleInput(NIN_BUTTON_SELECT, 1);
        break;
    case Qt::Key_Left:
        _emu.handleInput(NIN_BUTTON_LEFT, 1);
        break;
    case Qt::Key_Right:
        _emu.handleInput(NIN_BUTTON_RIGHT, 1);
        break;
    case Qt::Key_Up:
        _emu.handleInput(NIN_BUTTON_UP, 1);
        break;
    case Qt::Key_Down:
        _emu.handleInput(NIN_BUTTON_DOWN, 1);
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Z:
        _emu.handleInput(NIN_BUTTON_A, 0);
        break;
    case Qt::Key_X:
        _emu.handleInput(NIN_BUTTON_B, 0);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        _emu.handleInput(NIN_BUTTON_START, 0);
        break;
    case Qt::Key_Space:
        _emu.handleInput(NIN_BUTTON_SELECT, 0);
        break;
    case Qt::Key_Left:
        _emu.handleInput(NIN_BUTTON_LEFT, 0);
        break;
    case Qt::Key_Right:
        _emu.handleInput(NIN_BUTTON_RIGHT, 0);
        break;
    case Qt::Key_Up:
        _emu.handleInput(NIN_BUTTON_UP, 0);
        break;
    case Qt::Key_Down:
        _emu.handleInput(NIN_BUTTON_DOWN, 0);
        break;
    }
}

void MainWindow::updateTexture(const char* texture)
{
    _render->updateTexture(texture);
    _render->update();
}

void MainWindow::openRom()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("&Open ROM"), "", "*.nes");
    if (!filename.isEmpty())
    {
        QByteArray rawFilename = filename.toUtf8();
        _emu.loadRom(rawFilename.data());
    }
}

void MainWindow::createActions()
{
    _openRom = new QAction(tr("&Open ROM"), this);
    _openRom->setShortcut(QKeySequence::Open);
    connect(_openRom, SIGNAL(triggered(void)), this, SLOT(openRom(void)));

    _pauseEmulation = new QAction(tr("&Pause"), this);
    connect(_pauseEmulation, SIGNAL(triggered(void)), &_emu, SLOT(pause(void)));

    _resumeEmulation = new QAction(tr("&Resume"), this);
    connect(_resumeEmulation, SIGNAL(triggered(void)), &_emu, SLOT(resume(void)));
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_openRom);

    _emulationMenu = menuBar()->addMenu(tr("&Emulation"));
    _emulationMenu->addAction(_pauseEmulation);
    _emulationMenu->addAction(_resumeEmulation);
}