/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
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

#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <cstdint>
#include <mutex>

class RenderWidget : public QOpenGLWidget
{
    Q_OBJECT;

public:
    explicit RenderWidget(QWidget* parent = nullptr);
    virtual ~RenderWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int j) override;

    void setFit(bool fit);
    void setIntegerScale(bool integerScale);
    void setPixelAspectRatio(float pixelAspectRatio);
    void setOverscan(int x, int y);

    void updateTexture(const char* texture);

private:
    void computeViewBox();

    std::mutex _mutex;
    GLuint     _texture;
    char       _rawTexture[256 * 240 * 4];
    bool       _fit;
    bool       _integerScale;
    float      _pixelAspectRatio;
    int        _xOverscan;
    int        _yOverscan;
    float      _xMin;
    float      _xMax;
    float      _yMin;
    float      _yMax;
    float      _texXMin;
    float      _texXMax;
    float      _texYMin;
    float      _texYMax;
};

#endif
