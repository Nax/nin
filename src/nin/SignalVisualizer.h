#ifndef SIGNAL_VISUALIZER_H
#define SIGNAL_VISUALIZER_H

#include <mutex>
#include <cstdint>
#include <QWidget>
#include <QOpenGLWidget>

class SignalVisualizer : public QOpenGLWidget
{
    Q_OBJECT;

public:
    explicit SignalVisualizer(size_t sampleCount, QWidget* parent = nullptr);
    virtual ~SignalVisualizer();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int j) override;

    bool dirty() const;

    void setSamples(const float* samples);
    void setResolution(float min, float max);
    void setColor(float r, float g, float b);
    void setBackgroundColor(float r, float g, float b);

private:
    std::mutex      _mutex;
    size_t          _sampleCount;
    float*          _samples;
    float           _min;
    float           _max;
    float           _color[3];
    float           _colorBg[3];
    bool            _dirty;
};

#endif
