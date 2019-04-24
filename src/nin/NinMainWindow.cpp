#include <QKeyEvent>
#include "NinMainWindow.h"
#include "NinEmulator.h"

NinMainWindow::NinMainWindow(NinEmulator& emu, QWindow* parent)
: QOpenGLWindow(UpdateBehavior::NoPartialUpdate, parent)
, _emu(emu)
{
    static const int scale = 3;

    resize(256 * scale, 240 * scale);
    setTitle("Nin");
}

NinMainWindow::~NinMainWindow()
{

}

void NinMainWindow::initializeGL()
{
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void NinMainWindow::paintGL()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, 1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, -1.f);
    glEnd();
}

void NinMainWindow::keyPressEvent(QKeyEvent* event)
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

void NinMainWindow::keyReleaseEvent(QKeyEvent* event)
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

void NinMainWindow::update(const char* texture)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 240, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
    QOpenGLWindow::update();
}
