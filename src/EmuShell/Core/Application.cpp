#include <EmuShell/Core/Application.h>

using namespace EmuShell;

Application::Application()
{
    init();
}

Application::~Application()
{
    deinit();
}
