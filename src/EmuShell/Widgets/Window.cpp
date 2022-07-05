#include <EmuShell/Widgets/Window.h>
#include <EmuShell/Util/FreeList.h>
#include <EmuShell/Core/Application.h>

using namespace EmuShell;

namespace
{

int             sWindowCount;
FreeList<int>   sWindowAllocator;

}

Window::Window()
{
    _id = sWindowAllocator.alloc();
    init();
    sWindowCount++;
}

Window::~Window()
{
    deinit();
    sWindowAllocator.dealloc(_id);
    sWindowCount--;
    if (!sWindowCount)
        quit(0);
}
