#ifndef MEMORY_WINDOW_H
#define MEMORY_WINDOW_H

#include <cstdint>
#include <QWidget>
#include <QTimer>

#include <nin/nin.h>
#include "HexView.h"

class MemoryWindow : public QWidget
{
    Q_OBJECT;

public:
    explicit MemoryWindow(QWidget* parent = nullptr);

public slots:
    void refresh(NinState* state);

private:
    HexView*    _hexView;
};

#endif
