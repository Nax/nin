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

#ifndef SIGNAL_VISUALIZER_H
#define SIGNAL_VISUALIZER_H

#include <QOpenGLWidget>
#include <QWidget>
#include <cstdint>
#include <mutex>

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
    std::mutex _mutex;
    size_t     _sampleCount;
    float*     _samples;
    float      _min;
    float      _max;
    float      _color[3];
    float      _colorBg[3];
    bool       _dirty;
};

#endif
