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

#ifndef DEBUGGER_WINDOW_H
#define DEBUGGER_WINDOW_H

#include <QSpinBox>
#include <QWidget>
#include <cstdint>

#include <nin/nin.h>

class DisassemblerWidget;
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

    DisassemblerWidget* _disasm;
};

#endif
