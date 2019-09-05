#ifndef MEMORY_WINDOW_H
#define MEMORY_WINDOW_H

#include <cstdint>
#include <QWidget>
#include <QTimer>

#include <nin/nin.h>
#include "HexView.h"

class Emulator;
class MemoryWindow : public QWidget
{
    Q_OBJECT;

public:
    explicit MemoryWindow(Emulator& emu, QWidget* parent = nullptr);
    virtual ~MemoryWindow();

private slots:
    void refresh(NinState* state);

private:
    Emulator&   _emu;
    HexView*    _hexView;
};

#endif
