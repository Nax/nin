/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QtWidgets>
#include <NinEmu/UI/SignalVisualizer.h>

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
        std::unique_lock<std::mutex> lock(_mutex);

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
    std::unique_lock<std::mutex> lock(_mutex);

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
