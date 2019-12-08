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

#include <QtWidgets>
#include <NinEmu/UI/DisassemblerWidget.h>
#include <NinEmu/Window/DebuggerWindow.h>

DebuggerWindow::DebuggerWindow(QWidget* parent)
: QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;
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
    regLayout->setRowStretch(6, 1);
    regLayout->setColumnMinimumWidth(0, 60);
    regLayout->setColumnMinimumWidth(1, 100);

    layout->addLayout(regLayout);
    _disasm = new DisassemblerWidget;
    layout->addWidget(_disasm, 1);

    setLayout(layout);
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
