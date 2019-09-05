#ifndef HEX_VIEW_H
#define HEX_VIEW_H

#include <cstdint>
#include <QWidget>
#include <QPainter>
#include <QAbstractScrollArea>

class HexView : public QAbstractScrollArea
{
    Q_OBJECT;

public:
    HexView(size_t bufferSize, QWidget* parent = nullptr);
    virtual ~HexView();

    uint8_t* buffer();

    virtual void paintEvent(QPaintEvent* event) override;

private:
    QWidget*    _viewport;
    size_t      _bufferSize;
    uint8_t*    _buffer;
};

#endif
