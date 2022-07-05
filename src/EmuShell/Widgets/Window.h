#ifndef EMUSHELL_INCLUDED_WIDGETS_WINDOW_H
#define EMUSHELL_INCLUDED_WIDGETS_WINDOW_H

#include <cstdint>
#include <EmuShell/Core/Arch.h>
#include <EmuShell/Core/NonCopyable.h>

namespace EmuShell
{

class Window : private NonCopyable
{
public:
    Window();
    virtual ~Window();

    void hide();
    void show();
    virtual void close();

private:
    void init();
    void deinit();

    int _id;
};

}


#endif /* EMUSHELL_INCLUDED_WINDOW_H */
