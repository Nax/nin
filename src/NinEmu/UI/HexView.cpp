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

#include <NinEmu/UI/HexView.h>
#include <QFontDatabase>
#include <QScrollBar>

#define LINE_HEIGHT (16)

HexView::HexView(const uint8_t* buffer, size_t bufferSize, QWidget* parent)
: QAbstractScrollArea(parent)
, _bufferSize(bufferSize)
, _buffer(buffer)
{
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(f);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    setFixedWidth(80 + 17 * 20);

    _viewport = new QWidget();
    setViewport(_viewport);

    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(16);
    updateScroll();

    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));
}

void HexView::paintEvent(QPaintEvent* event)
{
    QPainter painter(_viewport);
    uint16_t addr;
    char     tmp[7];
    int      offset;
    int      lineCount;

    offset    = verticalScrollBar()->value();
    lineCount = size().height() / LINE_HEIGHT;

    for (int j = 0; j < lineCount; ++j)
    {
        painter.setPen(Qt::blue);
        addr = (j + offset) << 4;
        snprintf(tmp, sizeof(tmp), "0x%04X", addr);
        painter.drawText(QPoint(5, j * LINE_HEIGHT + 14), tmp);
        painter.setPen(Qt::black);

        for (unsigned i = 0; i < 16; ++i)
        {
            snprintf(tmp, sizeof(tmp), "%02X", _buffer[addr]);
            painter.drawText(QPoint(80 + i * 20, j * LINE_HEIGHT + 14), tmp);
            addr++;
        }
    }

    QAbstractScrollArea::paintEvent(event);
}

void HexView::resizeEvent(QResizeEvent* event)
{
    QAbstractScrollArea::resizeEvent(event);
    updateScroll();
}

void HexView::updateScroll()
{
    int maxScroll;

    maxScroll = (int)(_bufferSize / 16);
    maxScroll -= (_viewport->size().height() / LINE_HEIGHT);

    if (maxScroll < 0)
        maxScroll = 0;

    verticalScrollBar()->setMaximum(maxScroll);
    update();
}
