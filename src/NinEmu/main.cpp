#include <EmuShell/Main.h>
#include <EmuShell/Core/Application.h>
#include <EmuShell/Widgets/Window.h>

using namespace EmuShell;

int main(int argc, char** argv)
{
    Application app;
    Window win;
    win.show();
    return app.run();
}
