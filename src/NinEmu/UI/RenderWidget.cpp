/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 *
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 *
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 *
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <NinEmu/UI/RenderWidget.h>
#include <cmath>

RenderWidget::RenderWidget(QWidget* parent)
: QOpenGLWidget{parent}
, _texture{}
, _rawTexture{}
, _fit{}
, _integerScale{}
, _pixelAspectRatio{1.0f}
, _xOverscan{}
, _yOverscan{}
{
    memset(_rawTexture, 0, 256 * 240 * 4);
    computeViewBox();
}

RenderWidget::~RenderWidget()
{
}

void RenderWidget::initializeGL()
{
    static const char black[4] = {0x00, 0x00, 0x00, 0x00};

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
    (void)w;
    (void)h;
    std::unique_lock<std::mutex> lock(_mutex);

    computeViewBox();
}

void RenderWidget::setPixelAspectRatio(float pixelAspectRatio)
{
    std::unique_lock<std::mutex> lock(_mutex);

    _pixelAspectRatio = pixelAspectRatio;
    computeViewBox();
}

void RenderWidget::setFit(bool fit)
{
    std::unique_lock<std::mutex> lock(_mutex);

    _fit = fit;
    computeViewBox();
}

void RenderWidget::setIntegerScale(bool integerScale)
{
    std::unique_lock<std::mutex> lock(_mutex);

    _integerScale = integerScale;
    computeViewBox();
}

void RenderWidget::setOverscan(int x, int y)
{
    std::unique_lock<std::mutex> lock(_mutex);

    _xOverscan = x;
    _yOverscan = y;
    computeViewBox();
}

void RenderWidget::updateTexture(const char* texture)
{
    std::unique_lock<std::mutex> lock(_mutex);

    memcpy(_rawTexture, texture, 256 * 240 * 4);
    QOpenGLWidget::update();
}

void RenderWidget::computeViewBox()
{
    int   w;
    int   h;
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
        _xMax = 1.f;
        _yMin = -1.f;
        _yMax = 1.f;
    }
    else
    {
        w      = size().width();
        h      = size().height();
        imageW = ((256.f - 2 * _xOverscan) * _pixelAspectRatio);
        imageH = (240.f - 2 * _yOverscan);
        xRatio = (float)w / imageW;
        yRatio = (float)h / imageH;

        if (yRatio > xRatio)
        {
            r         = xRatio / yRatio;
            _xMin     = -1.f;
            _xMax     = 1.f;
            _yMin     = -r;
            _yMax     = r;
            amplitude = xRatio;
        }
        else
        {
            r         = yRatio / xRatio;
            _xMin     = -r;
            _xMax     = r;
            _yMin     = -1.f;
            _yMax     = 1.f;
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
