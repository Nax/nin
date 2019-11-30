#include <QtWidgets>
#include "DebuggerWindow.h"
#include "DisassemblerWidget.h"

DebuggerWindow::DebuggerWindow(QWidget* parent)
: QWidget(parent)
{
    QGridLayout* regLayout = new QGridLayout;

    regLayout->addWidget(new QLabel("PC"), 0, 0);
    _spinPC = new QSpinBox;
    _spinPC->setMinimum(0);
    _spinPC->setMaximum(0xffff);
    _spinPC->setDisplayIntegerBase(16);
    _spinPC->setPrefix("0x");
    regLayout->addWidget(_spinPC, 0, 1);

    regLayout->addWidget(new QLabel("A"),  1, 0);
    _spinA = new QSpinBox;
    _spinA->setMinimum(0);
    _spinA->setMaximum(0xff);
    _spinA->setDisplayIntegerBase(16);
    _spinA->setPrefix("0x");
    regLayout->addWidget(_spinA, 1, 1);

    regLayout->addWidget(new QLabel("X"),  2, 0);
    _spinX = new QSpinBox;
    _spinX->setMinimum(0);
    _spinX->setMaximum(0xff);
    _spinX->setDisplayIntegerBase(16);
    _spinX->setPrefix("0x");
    regLayout->addWidget(_spinX, 2, 1);

    regLayout->addWidget(new QLabel("Y"),  3, 0);
    _spinY = new QSpinBox;
    _spinY->setMinimum(0);
    _spinY->setMaximum(0xff);
    _spinY->setDisplayIntegerBase(16);
    _spinY->setPrefix("0x");
    regLayout->addWidget(_spinY, 3, 1);

    regLayout->addWidget(new QLabel("S"),  4, 0);
    _spinS = new QSpinBox;
    _spinS->setMinimum(0);
    _spinS->setMaximum(0xff);
    _spinS->setDisplayIntegerBase(16);
    _spinS->setPrefix("0x");
    regLayout->addWidget(_spinS, 4, 1);

    _disasm = new DisassemblerWidget;
    regLayout->addWidget(_disasm, 0, 2, 5, 5);

    setLayout(regLayout);
    setWindowTitle("Debugger");
}

void DebuggerWindow::refresh(NinState* state)
{
    uint16_t pc;
    uint8_t buffer[0x200];
    NinInt32 tmp;

    ninInfoQueryInteger(state, &tmp, NIN_INFO_PC);
    pc = tmp;
    _spinPC->setValue(tmp);

    ninInfoQueryInteger(state, &tmp, NIN_INFO_REG_A);
    _spinA->setValue(tmp);

    ninInfoQueryInteger(state, &tmp, NIN_INFO_REG_X);
    _spinX->setValue(tmp);

    ninInfoQueryInteger(state, &tmp, NIN_INFO_REG_Y);
    _spinY->setValue(tmp);

    ninInfoQueryInteger(state, &tmp, NIN_INFO_REG_S);
    _spinS->setValue(tmp);

    ninDumpMemory(state, buffer, pc, sizeof(buffer));
    _disasm->disassemble(pc, buffer, sizeof(buffer));
}
