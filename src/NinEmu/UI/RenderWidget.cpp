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

#include <cmath>
#include <NinEmu/UI/RenderWidget.h>

RenderWidget::RenderWidget(QWidget* parent)
: QOpenGLWidget(parent)
, _texture(0)
, _pixelAspectRatio(1.0f)
, _fit(false)
, _xOverscan(0)
, _yOverscan(0)
{
    memset(_rawTexture, 0, 256 * 240 * 4);
    computeViewBox();
}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::initializeGL()
{
    static const char black[4] = { 0x00, 0x00, 0x00, 0x00 };

    makeCurrent();
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, black);
}

void RenderWidget::paintGL()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, _texture);

    _mutex.lock();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 240, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, _rawTexture);

    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glTexCoord2f(_texXMin, _texYMax);
    glVertex2f(_xMin, _yMin);
    glTexCoord2f(_texXMin, _texYMin);
    glVertex2f(_xMin, _yMax);
    glTexCoord2f(_texXMax, _texYMin);
    glVertex2f(_xMax, _yMax);
    glTexCoord2f(_texXMax, _texYMax);
    glVertex2f(_xMax, _yMin);
    _mutex.unlock();

    glEnd();
}

void RenderWidget::resizeGL(int w, int h)
{
    std::unique_lock lock(_mutex);

    computeViewBox();
}

void RenderWidget::setPixelAspectRatio(float pixelAspectRatio)
{
    std::unique_lock lock(_mutex);

    _pixelAspectRatio = pixelAspectRatio;
    computeViewBox();
}

void RenderWidget::setFit(bool fit)
{
    std::unique_lock lock(_mutex);

    _fit = fit;
    computeViewBox();
}

void RenderWidget::setIntegerScale(bool integerScale)
{
    std::unique_lock lock(_mutex);

    _integerScale = integerScale;
    computeViewBox();
}

void RenderWidget::setOverscan(int x, int y)
{
    std::unique_lock lock(_mutex);

    _xOverscan = x;
    _yOverscan = y;
    computeViewBox();
}

void RenderWidget::updateTexture(const char* texture)
{
    std::unique_lock lock(_mutex);

    memcpy(_rawTexture, texture, 256 * 240 * 4);
    QOpenGLWidget::update();
}

void RenderWidget::computeViewBox()
{
    int w;
    int h;
    float imageW;
    float imageH;
    float xRatio;
    float yRatio;
    float amplitude;
    float tx;
    float ty;
    float r;
    float snap;

    /* Compute the overscan texture */
    tx = 1.f / 256.f;
    ty = 1.f / 240.f;

    _texXMin = _xOverscan * tx;
    _texXMax = 1.f - (_xOverscan * tx);
    _texYMin = _yOverscan * ty;
    _texYMax = 1.f - (_yOverscan * ty);

    if (_fit)
    {
        _xMin = -1.f;
        _xMax =  1.f;
        _yMin = -1.f;
        _yMax =  1.f;
    }
    else
    {
        w = size().width();
        h = size().height();
        imageW = ((256.f - 2 * _xOverscan) * _pixelAspectRatio);
        imageH = (240.f - 2 * _yOverscan);
        xRatio = (float)w / imageW;
        yRatio = (float)h / imageH;

        if (yRatio > xRatio)
        {
            r = xRatio / yRatio;
            _xMin = -1.f;
            _xMax =  1.f;
            _yMin = -r;
            _yMax =  r;
            amplitude = xRatio;
        }
        else
        {
            r = yRatio / xRatio;
            _xMin = -r;
            _xMax =  r;
            _yMin = -1.f;
            _yMax =  1.f;
            amplitude = yRatio;
        }

        if (_integerScale && amplitude > 1.f)
        {
            snap = std::floor(amplitude) / amplitude;
        }
        else
        {
            snap = 1.f;
        }

        _xMin *= snap;
        _xMax *= snap;
        _yMin *= snap;
        _yMax *= snap;
    }
}
