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

#include <QFontDatabase>
#include <QScrollBar>
#include <NinEmu/UI/DisassemblerWidget.h>

#define A_IMPL      0
#define A_IZX       1
#define A_Z         2
#define A_IMM       3
#define A_A         4
#define A_REL       5
#define A_IZY       6
#define A_ZX        7
#define A_AY        8
#define A_AX        9
#define A_IJMP      10
#define A_ZY        11

struct InstrDescription
{
    const char* name;
    int         addrMode;
};

static const InstrDescription kInstructions[256] = {
    // 0x00
    {"BRK ",    A_IMPL},
    {"ORA ",    A_IZX},
    {"STP*",    A_IMPL},
    {"SLO*",    A_IZX},
    {"NOP*",    A_Z},
    {"ORA ",    A_Z},
    {"ASL ",    A_Z},
    {"SLO*",    A_Z},
    {"PHP ",    A_IMPL},
    {"ORA ",    A_IMM},
    {"ASL ",    A_IMPL},
    {"ANC*",    A_IMM},
    {"NOP*",    A_A},
    {"ORA ",    A_A},
    {"ASL ",    A_A},
    {"SLO*",    A_A},
    {"BPL ",    A_REL},
    {"ORA ",    A_IZY},
    {"STP*",    A_IMM},
    {"SLO*",    A_IZY},
    {"NOP*",    A_ZX},
    {"ORA ",    A_ZX},
    {"ASL ",    A_ZX},
    {"SLO*",    A_ZX},
    {"CLC ",    A_IMPL},
    {"ORA ",    A_AY},
    {"NOP*",    A_IMPL},
    {"SLO*",    A_AY},
    {"NOP*",    A_AX},
    {"ORA ",    A_AX},
    {"ASL ",    A_AX},
    {"SLO*",    A_AX},

    // 0x20
    {"JSR ", A_A},
    {"AND ", A_IZX},
    {"STP*", A_IMPL},
    {"RLA*", A_IZX},
    {"BIT ", A_Z},
    {"AND ", A_Z},
    {"ROL ", A_Z},
    {"RLA*", A_Z},
    {"PLP ", A_IMPL},
    {"AND ", A_IMM},
    {"ROL ", A_IMPL},
    {"ANC*", A_IMM},
    {"BIT ", A_A},
    {"AND ", A_A},
    {"ROL ", A_A},
    {"RLA*", A_A},
    {"BMI ", A_REL},
    {"AND ", A_IZY},
    {"STP*", A_IMPL},
    {"RLA*", A_IZY},
    {"NOP*", A_ZX},
    {"AND ", A_ZX},
    {"ROL ", A_ZX},
    {"RLA*", A_ZX},
    {"SEC ", A_IMPL},
    {"AND ", A_AY},
    {"NOP*", A_IMPL},
    {"RLA*", A_AY},
    {"NOP*", A_AX},
    {"AND ", A_AX},
    {"ROL ", A_AX},
    {"RLA*", A_AX},

    // 0x40
    {"RTI ", A_IMPL},
    {"EOR ", A_IZX},
    {"STP*", A_IMPL},
    {"SRE*", A_IZX},
    {"NOP*", A_Z},
    {"EOR ", A_Z},
    {"LSR ", A_Z},
    {"SRE*", A_Z},
    {"PHA ", A_IMPL},
    {"EOR ", A_IMM},
    {"LSR ", A_IMPL},
    {"ALR*", A_IMM},
    {"JMP ", A_A},
    {"EOR ", A_A},
    {"LSR ", A_A},
    {"SRE*", A_A},
    {"BVC ", A_REL},
    {"EOR ", A_IZY},
    {"STP*", A_IMPL},
    {"SRE*", A_IZY},
    {"NOP*", A_ZX},
    {"EOR ", A_ZX},
    {"LSR ", A_ZX},
    {"SRE*", A_ZX},
    {"CLI ", A_IMPL},
    {"EOR ", A_AY},
    {"NOP*", A_IMPL},
    {"SRE*", A_AY},
    {"NOP*", A_AX},
    {"EOR ", A_AX},
    {"LSR ", A_AX},
    {"SRE*", A_AX},

    // 0x60
    { "RTS ", A_IMPL },
    { "ADC ", A_IZX },
    { "STP*", A_IMPL },
    { "RRA*", A_IZX },
    { "NOP*", A_Z },
    { "ADC ", A_Z },
    { "ROR ", A_Z },
    { "RRA*", A_Z },
    { "PLA ", A_IMPL },
    { "ADC ", A_IMM },
    { "ROR ", A_IMPL },
    { "ARR*", A_IMM },
    { "JMP ", A_IJMP },
    { "ADC ", A_A },
    { "ROR ", A_A },
    { "RRA*", A_A },
    { "BVS ", A_REL },
    { "ADC ", A_IZY },
    { "STP*", A_IMPL },
    { "RRA*", A_IZY },
    { "NOP*", A_ZX },
    { "ADC ", A_ZX },
    { "ROR ", A_ZX },
    { "RRA*", A_ZX },
    { "SEI ", A_IMPL },
    { "ADC ", A_AY },
    { "NOP*", A_IMPL },
    { "RRA*", A_AY },
    { "NOP*", A_AX },
    { "ADC ", A_AX },
    { "ROR ", A_AX },
    { "RRA*", A_AX },

    // 0x80
    { "NOP*", A_IMM },
    { "STA ", A_IZX },
    { "NOP*", A_IMM },
    { "SAX*", A_IZX },
    { "STY ", A_Z },
    { "STA ", A_Z },
    { "STX ", A_Z },
    { "SAX*", A_Z },
    { "DEY ", A_IMPL },
    { "NOP*", A_IMM },
    { "TXA ", A_IMPL },
    { "XAA*", A_IMM },
    { "STY ", A_A },
    { "STA ", A_A },
    { "STX ", A_A },
    { "SAX*", A_A },
    { "BCC ", A_REL },
    { "STA ", A_IZY },
    { "STP*", A_IMPL },
    { "AHX*", A_IZY },
    { "STY ", A_ZX },
    { "STA ", A_ZX },
    { "STX ", A_ZY },
    { "SAX*", A_ZY },
    { "TYA ", A_IMPL },
    { "STA ", A_AY },
    { "TXS ", A_IMPL },
    { "TAS*", A_AY },
    { "SHY*", A_AX },
    { "STA ", A_AX },
    { "SHX*", A_AY },
    { "AHX*", A_AY },

    // 0xA0
    { "LDY ", A_IMM },
    { "LDA ", A_IZX },
    { "LDX ", A_IMM },
    { "LAX*", A_IZX },
    { "LDY ", A_Z },
    { "LDA ", A_Z },
    { "LDX ", A_Z },
    { "LAX*", A_Z },
    { "TAY ", A_IMPL },
    { "LDA ", A_IMM },
    { "TAX ", A_IMPL },
    { "LAX*", A_IMM },
    { "LDY ", A_A },
    { "LDA ", A_A },
    { "LDX ", A_A },
    { "LAX*", A_A },
    { "BCS ", A_REL },
    { "LDA ", A_IZY },
    { "STP*", A_IMPL },
    { "LAX*", A_IZY },
    { "LDY ", A_ZX },
    { "LDA ", A_ZX },
    { "LDX ", A_ZY },
    { "LAX*", A_ZY },
    { "CLV ", A_IMPL },
    { "LDA ", A_AY },
    { "TSX ", A_IMPL },
    { "LAS*", A_AY },
    { "LDY ", A_AX },
    { "LDA ", A_AX },
    { "LDX ", A_AY },
    { "LAX*", A_AY },

    // 0xC0
    { "CPY ", A_IMM },
    { "CMP ", A_IZX },
    { "NOP*", A_IMM },
    { "DCP*", A_IZX },
    { "CPY ", A_Z },
    { "CMP ", A_Z },
    { "DEC ", A_Z },
    { "DCP*", A_Z },
    { "INY ", A_IMPL },
    { "CMP ", A_IMM },
    { "DEX ", A_IMPL },
    { "AXS*", A_IMM },
    { "CPY ", A_A },
    { "CMP ", A_A },
    { "DEC ", A_A },
    { "DCP*", A_A },
    { "BNE ", A_REL },
    { "CMP ", A_IZY },
    { "STP*", A_IMPL },
    { "DCP*", A_IZY },
    { "NOP*", A_ZX },
    { "CMP ", A_ZX },
    { "DEC ", A_ZX },
    { "DCP*", A_ZX },
    { "CLD ", A_IMPL },
    { "CMP ", A_AY },
    { "NOP*", A_IMPL },
    { "DCP*", A_AY },
    { "NOP*", A_AX },
    { "CMP ", A_AX },
    { "DEC ", A_AX },
    { "DCP*", A_AX },

    // 0xE0
    { "CPX ", A_IMM },
    { "SBC ", A_IZX },
    { "NOP*", A_IMM },
    { "ISC*", A_IZX },
    { "CPX ", A_Z },
    { "SBC ", A_Z },
    { "INC ", A_Z },
    { "ISC*", A_Z },
    { "INX ", A_IMPL },
    { "SBC ", A_IMM },
    { "NOP ", A_IMPL },
    { "SBC*", A_IMM },
    { "CPX ", A_A },
    { "SBC ", A_A },
    { "INC ", A_A },
    { "ISC*", A_A },
    { "BEQ ", A_REL },
    { "SBC ", A_IZY },
    { "STP*", A_IMPL },
    { "ISC*", A_IZY },
    { "NOP*", A_ZX },
    { "SBC ", A_ZX },
    { "INC ", A_ZX },
    { "ISC*", A_ZX },
    { "SED ", A_IMPL },
    { "SBC ", A_AY },
    { "NOP*", A_IMPL },
    { "ISC*", A_AY },
    { "NOP*", A_AX },
    { "SBC ", A_AX },
    { "INC ", A_AX },
    { "ISC*", A_AX },
};

DisassemblerWidget::DisassemblerWidget(QWidget* parent)
: QAbstractScrollArea(parent)
{
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(f);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    _viewport = new QWidget();
    setViewport(_viewport);

    memset(_buffer, 0, sizeof(_buffer));

    _viewport->setFixedSize(80 + 16 * 20, 64 * 16 + 64);

    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(1);
    verticalScrollBar()->setMaximum(64 / 16);

    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));
}

void DisassemblerWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(_viewport);
    uint16_t addr;
    char tmp[7];

    int lineCount = size().height() / 16;

    for (int j = 0; j < lineCount; ++j)
    {
        painter.setPen(Qt::blue);
        addr = _buffer[j].addr;
        snprintf(tmp, sizeof(tmp), "0x%04X", addr);
        painter.drawText(QPoint(0, j * 16 + 14), tmp);

        painter.setPen(Qt::gray);
        for (int i = 0; i < _buffer[j].rawCount; ++i)
        {
            snprintf(tmp, sizeof(tmp), "%02X", (unsigned)_buffer[j].raw[i]);
            painter.drawText(QPoint(60 + i * 16, j * 16 + 14), tmp);
        }

        painter.setPen(Qt::black);
        painter.drawText(QPoint(160, j * 16 + 14), _buffer[j].str);
    }

    QAbstractScrollArea::paintEvent(event);
}

void DisassemblerWidget::disassemble(uint16_t pc, const uint8_t* data, size_t dataSize)
{
    size_t off;

    memset(_buffer, 0, sizeof(_buffer));
    off = 0;
    for (int i = 0; i < 64; ++i)
    {
        DisasmInstr& instr = _buffer[i];
        off += disassembleInstr(instr, (pc + off) & 0xffff, data + off);
    }

    update();
}

std::uint8_t DisassemblerWidget::disassembleInstr(DisasmInstr& instr, uint16_t pc, const uint8_t* data)
{
    const InstrDescription* desc;
    char tmp[32];

    instr.addr = pc;
    instr.raw[0] = data[0];
    instr.raw[1] = data[1];
    instr.raw[2] = data[2];

    desc = kInstructions + data[0];
    switch (desc->addrMode)
    {
    case A_IMPL:
        instr.rawCount = 1;
        snprintf(tmp, sizeof(tmp), "%s", desc->name);
        break;

    case A_IZX:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s ($%02X,X)", desc->name, instr.raw[1]);
        break;

    case A_Z:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s $%02X", desc->name, instr.raw[1]);
        break;

    case A_IMM:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s #$%02X", desc->name, instr.raw[1]);
        break;

    case A_A:
        instr.rawCount = 3;
        snprintf(tmp, sizeof(tmp), "%s $%02X%02X", desc->name, instr.raw[2], instr.raw[1]);
        break;

    case A_REL:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s $%04X", desc->name, (pc + (int8_t)instr.raw[1] + 2) & 0xffff);
        break;

    case A_IZY:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s ($%02X),Y", desc->name, instr.raw[1]);
        break;

    case A_ZX:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s $%02X,X", desc->name, instr.raw[1]);
        break;

    case A_AY:
        instr.rawCount = 3;
        snprintf(tmp, sizeof(tmp), "%s $%02X%02X,Y", desc->name, instr.raw[2], instr.raw[1]);
        break;

    case A_AX:
        instr.rawCount = 3;
        snprintf(tmp, sizeof(tmp), "%s $%02X%02X,X", desc->name, instr.raw[2], instr.raw[1]);
        break;

    case A_IJMP:
        instr.rawCount = 3;
        snprintf(tmp, sizeof(tmp), "%s ($%02X%02X)", desc->name, instr.raw[2], instr.raw[1]);
        break;

    case A_ZY:
        instr.rawCount = 2;
        snprintf(tmp, sizeof(tmp), "%s $%02X,Y", desc->name, instr.raw[1]);
        break;
    }

    strcpy(instr.str, tmp);
    return instr.rawCount;
}
