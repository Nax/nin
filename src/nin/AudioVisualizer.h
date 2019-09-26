#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include <mutex>
#include <cstdint>
#include <QWidget>
#include <QOpenGLWidget>

class AudioVisualizer : public QOpenGLWidget
{
    Q_OBJECT;

public:
    explicit AudioVisualizer(QWidget* parent = nullptr);

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int j) override;

    void pushSamples(const int16_t* samples);

private:
    std::mutex      _mutex;
    int16_t         _samples[1024];
};

#endif
