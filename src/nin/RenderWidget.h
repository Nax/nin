#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <cstdint>
#include <QWidget>
#include <QOpenGLWidget>

class Emulator;
class RenderWidget : public QOpenGLWidget
{
    Q_OBJECT;

public:
    explicit RenderWidget(Emulator& emu, QWidget* parent = nullptr);
    virtual ~RenderWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int j) override;

    void updateTexture(const char* texture);

private:
    Emulator&   _emu;
    GLuint      _texture;
};

#endif
