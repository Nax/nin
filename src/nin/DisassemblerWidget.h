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
