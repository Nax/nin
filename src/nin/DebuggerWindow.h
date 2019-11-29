#ifndef DEBUGGER_WINDOW_H
#define DEBUGGER_WINDOW_H

#include <cstdint>
#include <QWidget>
#include <QSpinBox>

#include <nin/nin.h>

class DebuggerWindow : public QWidget
{
    Q_OBJECT;

public:
    explicit DebuggerWindow(QWidget* parent = nullptr);

public slots:
    void refresh(NinState* state);

private:
    QSpinBox* _spinPC;
    QSpinBox* _spinA;
    QSpinBox* _spinX;
    QSpinBox* _spinY;
    QSpinBox* _spinS;
};

#endif
