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

void NinMainWindow::update(const char* texture)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 240, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
    QOpenGLWindow::update();
}
