#include <EmuShell/Core/Application.h>
#include <EmuShell/Core/Arch.h>

#define EMUSHELL_WM_DISPATCH        (WM_APP + 0)
#define EMUSHELL_WM_DISPATCH_SYNC   (WM_APP + 1)

using namespace EmuShell;

namespace
{

DWORD   sMainThreadID;
HANDLE  sSyncDispatchEvent;
HANDLE  sSyncDispatchMutex;

}

void Application::init()
{
    sMainThreadID = GetCurrentThreadId();
    sSyncDispatchEvent = CreateEvent(nullptr, 0, 0, nullptr);
    sSyncDispatchMutex = CreateMutex(nullptr, 0, nullptr);
}

void Application::deinit()
{

}

int Application::run()
{
    MSG msg;
    DispatchCallback cb;
    void* cbArg;

    for (;;)
    {
        GetMessage(&msg, nullptr, 0, 0);
        switch (msg.message)
        {
        case WM_QUIT:
            return (int)msg.wParam;
        case EMUSHELL_WM_DISPATCH:
        case EMUSHELL_WM_DISPATCH_SYNC:
            cb = (DispatchCallback)msg.wParam;
            cbArg = (void*)msg.lParam;
            cb(cbArg);
            if (msg.message == EMUSHELL_WM_DISPATCH_SYNC)
                SetEvent(sSyncDispatchEvent);
            break;
        default:
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void EmuShell::dispatch(DispatchCallback cb, void* arg)
{
    if (GetCurrentThreadId() == sMainThreadID)
    {
        cb(arg);
    }
    else
    {
        PostMessage(nullptr, EMUSHELL_WM_DISPATCH, (WPARAM)cb, (LPARAM)arg);
    }
}

void EmuShell::dispatchSync(DispatchCallback cb, void* arg)
{
    if (GetCurrentThreadId() == sMainThreadID)
    {
        cb(arg);
    }
    else
    {
        WaitForSingleObject(sSyncDispatchMutex, INFINITE);
        PostMessage(nullptr, EMUSHELL_WM_DISPATCH_SYNC, (WPARAM)cb, (LPARAM)arg);
        WaitForSingleObject(sSyncDispatchEvent, INFINITE);
        ReleaseMutex(sSyncDispatchMutex);
    }
}
