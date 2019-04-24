#ifndef NIN_MAIN_WINDOW_H
#define NIN_MAIN_WINDOW_H

#include <QOpenGLWindow>

class NinEmulator;
class NinMainWindow : public QOpenGLWindow
{
    Q_OBJECT;
public:
    explicit NinMainWindow(NinEmulator& emu, QWindow* parent = nullptr);
    ~NinMainWindow();

private:
    NinEmulator&    _emu;
};

#endif
