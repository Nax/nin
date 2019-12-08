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

#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <mutex>
#include <cstdint>
#include <QWidget>
#include <QOpenGLWidget>

class RenderWidget : public QOpenGLWidget
{
    Q_OBJECT;

public:
    explicit RenderWidget(QWidget* parent = nullptr);
    virtual ~RenderWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int j) override;

    void setPixelAspectRatio(float pixelAspectRatio);
    void setFit(bool fit);
    void updateTexture(const char* texture);

private:
    void computeViewBox();

    std::mutex      _mutex;
    GLuint          _texture;
    char            _rawTexture[256 * 240 * 4];
    float           _pixelAspectRatio;
    bool            _fit;
    float           _xMin;
    float           _xMax;
    float           _yMin;
    float           _yMax;
};

#endif
