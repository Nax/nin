#include <QtWidgets>
#include "SignalVisualizer.h"

static float mapUnit(float min, float max, float v)
{
    return (((v - min) / (max - min)) * 2.f) - 1.f;
}

SignalVisualizer::SignalVisualizer(size_t sampleCount, QWidget* parent)
: QOpenGLWidget(parent)
, _sampleCount(sampleCount)
, _samples(new float[sampleCount])
, _min(-1.f)
, _max(1.f)
, _color{1.f, 1.f, 1.f}
, _colorBg{0.f, 0.f, 0.f}
, _dirty(false)
{
    QSurfaceFormat format;

    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setSamples(4);

    setFormat(format);

    memset(_samples, 0, _sampleCount * 4);
    setMinimumSize(512, 256);
}

SignalVisualizer::~SignalVisualizer()
{
    delete[] _samples;
}

void SignalVisualizer::initializeGL()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
}

void SignalVisualizer::paintGL()
{
    float x;
    float y;

    glClearColor(_colorBg[0], _colorBg[1], _colorBg[2], 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glColor3f(_color[0], _color[1], _color[2]);
    glBegin(GL_LINE_STRIP);
    {
        std::unique_lock lock(_mutex);

        for (int i = 0; i < _sampleCount; ++i)
        {
            x = mapUnit(0.f, (float)(_sampleCount - 1), (float)i);
            y = mapUnit(_min, _max, _samples[i]);
            glVertex2f(x, y);
        }
        _dirty = false;
    }
    glEnd();
}

void SignalVisualizer::resizeGL(int w, int h)
{
    (void)w;
    (void)h;
}

bool SignalVisualizer::dirty() const
{
    return _dirty;
}

void SignalVisualizer::setSamples(const float* samples)
{
    std::unique_lock lock(_mutex);

    if (!_dirty)
    {
        memcpy(_samples, samples, _sampleCount * sizeof(*_samples));
        _dirty = true;
        QWidget::update();
    }
}

void SignalVisualizer::setResolution(float min, float max)
{
    _min = min;
    _max = max;
}

void SignalVisualizer::setColor(float r, float g, float b)
{
    _color[0] = r;
    _color[1] = g;
    _color[2] = b;
}

void SignalVisualizer::setBackgroundColor(float r, float g, float b)
{
    _colorBg[0] = r;
    _colorBg[1] = g;
    _colorBg[2] = b;
}
