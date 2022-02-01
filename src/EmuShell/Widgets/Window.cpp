#include <EmuShell/Widgets/Window.h>
#include <EmuShell/Util/FreeList.h>

using namespace EmuShell;

namespace
{

FreeList<int> sWindowAllocator;

}

Window::Window()
{
    _id = sWindowAllocator.alloc();
    init();
}

Window::~Window()
{
    deinit();
    sWindowAllocator.dealloc(_id);
}
