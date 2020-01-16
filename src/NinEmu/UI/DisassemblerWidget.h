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

#ifndef DISASSEMBLER_WIDGET_H
#define DISASSEMBLER_WIDGET_H

#include <cstdint>
#include <QWidget>
#include <QPainter>
#include <QString>
#include <QAbstractScrollArea>

class DisassemblerWidget : public QAbstractScrollArea
{
    Q_OBJECT;

public:
    DisassemblerWidget(QWidget* parent = nullptr);

    virtual void paintEvent(QPaintEvent* event) override;
    void disassemble(uint16_t pc, const uint8_t* data, size_t dataSize);

private:
    struct DisasmInstr
    {
        uint16_t    addr;
        uint8_t     raw[3];
        uint8_t     rawCount;
        char        str[32];
    };

    size_t disassembleInstr(DisasmInstr& instr, uint16_t pc, const uint8_t* data);

    QWidget*        _viewport;
    DisasmInstr     _buffer[64];
};

#endif