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

#ifndef DISASSEMBLER_WIDGET_H
#define DISASSEMBLER_WIDGET_H

#include <QAbstractScrollArea>
#include <QPainter>
#include <QString>
#include <QWidget>
#include <cstdint>

class DisassemblerWidget : public QAbstractScrollArea
{
    Q_OBJECT;

public:
    DisassemblerWidget(QWidget* parent = nullptr);

    virtual void paintEvent(QPaintEvent* event) override;
    void         disassemble(uint16_t pc, const uint8_t* data, std::size_t dataSize);

private:
    struct DisasmInstr
    {
        uint16_t addr;
        uint8_t  raw[3];
        uint8_t  rawCount;
        char     str[32];
    };

    std::uint8_t disassembleInstr(DisasmInstr& instr, uint16_t pc, const uint8_t* data);

    QWidget*    _viewport;
    DisasmInstr _buffer[64];
};

#endif
