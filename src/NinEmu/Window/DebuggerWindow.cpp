/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
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

#include <NinEmu/UI/DisassemblerWidget.h>
#include <NinEmu/Window/DebuggerWindow.h>
#include <QtWidgets>

DebuggerWindow::DebuggerWindow(QWidget* parent)
: QWidget(parent)
{
    QHBoxLayout* layout    = new QHBoxLayout;
    QGridLayout* regLayout = new QGridLayout;

    regLayout->addWidget(new QLabel("PC"), 0, 0);
    _spinPC = new QSpinBox;
    _spinPC->setMinimum(0);
    _spinPC->setMaximum(0xffff);
    _spinPC->setDisplayIntegerBase(16);
    _spinPC->setPrefix("0x");
    regLayout->addWidget(_spinPC, 0, 1);

    regLayout->addWidget(new QLabel("A"), 1, 0);
    _spinA = new QSpinBox;
    _spinA->setMinimum(0);
    _spinA->setMaximum(0xff);
    _spinA->setDisplayIntegerBase(16);
    _spinA->setPrefix("0x");
    regLayout->addWidget(_spinA, 1, 1);

    regLayout->addWidget(new QLabel("X"), 2, 0);
    _spinX = new QSpinBox;
    _spinX->setMinimum(0);
    _spinX->setMaximum(0xff);
    _spinX->setDisplayIntegerBase(16);
    _spinX->setPrefix("0x");
    regLayout->addWidget(_spinX, 2, 1);

    regLayout->addWidget(new QLabel("Y"), 3, 0);
    _spinY = new QSpinBox;
    _spinY->setMinimum(0);
    _spinY->setMaximum(0xff);
    _spinY->setDisplayIntegerBase(16);
    _spinY->setPrefix("0x");
    regLayout->addWidget(_spinY, 3, 1);

    regLayout->addWidget(new QLabel("S"), 4, 0);
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
    uint8_t  buffer[0x200];
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
