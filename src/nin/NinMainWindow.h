#ifndef NIN_MAIN_WINDOW_H
#define NIN_MAIN_WINDOW_H

#include <cstdint>
#include <QOpenGLWindow>

class NinEmulator;
class NinMainWindow : public QOpenGLWindow
{
    Q_OBJECT;
public:
    explicit NinMainWindow(NinEmulator& emu, QWindow* parent = nullptr);
    ~NinMainWindow();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;

    void update(const char* texture);

private:
    NinEmulator&    _emu;
    GLuint          _texture;
};

#endif
