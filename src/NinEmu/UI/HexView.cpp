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

#include <QFontDatabase>
#include <QScrollBar>
#include <NinEmu/UI/HexView.h>

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
    char tmp[7];
    int offset;
    int lineCount;

    offset = verticalScrollBar()->value();
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
