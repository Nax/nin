#include <windows.h>
#include <vector>
#include <cstdint>
#include <EmuShell/Widgets/Window.h>
#include <EmuShell/Core/Application.h>

using namespace EmuShell;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace
{

std::vector<HWND>   sWindow;
std::vector<HDC>    sDC;
bool                sInit;
ATOM                sWinClass;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void registerWinClass()
{
    WNDCLASSEX wndClass{};
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_OWNDC;
    wndClass.lpfnWndProc = &WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = (HINSTANCE)&__ImageBase;
    wndClass.hIcon = nullptr;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = TEXT("EmuShellWindow");
    wndClass.hIconSm = nullptr;

    sWinClass = RegisterClassEx(&wndClass);

    sInit = true;
}

}

void Window::init()
{
    EmuShell::dispatchSync([] (void* arg)
    {
        int id = (int)(intptr_t)arg;

        if (!sInit)
            registerWinClass();

        if (sWindow.size() <= id)
        {
            sWindow.resize(id + 1);
            sDC.resize(id + 1);
        }

        sWindow[id] = CreateWindow(
            (LPTSTR)sWinClass,
            TEXT(""),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            800, 600,
            nullptr,
            nullptr,
            (HINSTANCE)&__ImageBase,
            0
        );
        sDC[id] = GetDC(sWindow[id]);
    }, (void*)(intptr_t)_id);
}

void Window::deinit()
{
    EmuShell::dispatchSync([] (void* arg)
    {
        int id = (int)(intptr_t)arg;
        ReleaseDC(sWindow[id], sDC[id]);
        DestroyWindow(sWindow[id]);
    }, (void*)(intptr_t)_id);
}

void Window::show()
{
    EmuShell::dispatchSync([] (void* arg)
    {
        int id = (int)(intptr_t)arg;
        ShowWindow(sWindow[id], 1);
    }, (void*)(intptr_t)_id);
}

void Window::hide()
{
    EmuShell::dispatchSync([] (void* arg)
    {
        int id = (int)(intptr_t)arg;
        ShowWindow(sWindow[id], 0);
    }, (void*)(intptr_t)_id);
}

void Window::close()
{
    EmuShell::dispatchSync([] (void* arg)
    {
        int id = (int)(intptr_t)arg;
        DestroyWindow(sWindow[id]);
    }, (void*)(intptr_t)_id);
}
