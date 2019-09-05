#include <QFontDatabase>
#include <QScrollBar>
#include "HexView.h"

HexView::HexView(size_t bufferSize, QWidget* parent)
: QAbstractScrollArea(parent)
, _bufferSize(bufferSize)
{
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(f);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    _viewport = new QWidget();
    setViewport(_viewport);

    _buffer = new uint8_t[bufferSize];
    memset(_buffer, 0, bufferSize);

    _viewport->setFixedSize(80 + 16 * 20, _bufferSize * 16 + 64);

    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(1);
    verticalScrollBar()->setMaximum(bufferSize / 16);
}

HexView::~HexView()
{
    delete[] _buffer;
}

uint8_t* HexView::buffer()
{
    return _buffer;
}

void HexView::paintEvent(QPaintEvent* event)
{
    QPainter painter(_viewport);
    uint16_t addr;
    char tmp[7];

    size_t lineCount = size().height() / 16;

    for (size_t j = 0; j < lineCount; ++j)
    {
        painter.setPen(Qt::blue);
        addr = j << 4;
        snprintf(tmp, sizeof(tmp), "0x%04X", addr);
        painter.drawText(QPoint(0, j * 16 + 14), tmp);
        painter.setPen(Qt::black);

        for (unsigned i = 0; i < 16; ++i)
        {
            snprintf(tmp, sizeof(tmp), "%02X", _buffer[addr]);
            painter.drawText(QPoint(80 + i * 20, j * 16 + 14), tmp);
            addr++;
        }
    }

    QAbstractScrollArea::paintEvent(event);
}
