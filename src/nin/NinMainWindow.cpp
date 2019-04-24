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
