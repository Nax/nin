#ifndef SIGNAL_VISUALIZER_H
#define SIGNAL_VISUALIZER_H

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
