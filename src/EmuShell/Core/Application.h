#ifndef EMUSHELL_INCLUDED_CORE_APPLICATION_H
#define EMUSHELL_INCLUDED_CORE_APPLICATION_H

#include <EmuShell/Core/NonCopyable.h>

namespace EmuShell
{

class Application : private NonCopyable
{
public:
    Application();
    ~Application();

    int run();

private:
    void init();
    void deinit();
};

using DispatchCallback = void (*)(void*);

void dispatch(DispatchCallback cb, void* arg);
void dispatchSync(DispatchCallback cb, void* arg);

void quit(int code = 0);

}

#endif
