#ifndef INCLUDED_LIBNIN_JIT_JIT_SYM_H
#define INCLUDED_LIBNIN_JIT_JIT_SYM_H 1

#define SYM_ADDR_IMPL   0
#define SYM_ADDR_IMM    1
#define SYM_ADDR_IZX    2
#define SYM_ADDR_Z      3
#define SYM_ADDR_A      4
#define SYM_ADDR_REL    5
#define SYM_ADDR_IZY    6
#define SYM_ADDR_ZX     7
#define SYM_ADDR_ZY     8
#define SYM_ADDR_AX     9
#define SYM_ADDR_AY     10
#define SYM_ADDR_IJMP   11

#define SYM_OP_BRK      0x00
#define SYM_OP_JSR      0x01
#define SYM_OP_RTI      0x02
#define SYM_OP_RTS      0x03
#define SYM_OP_NOP      0x04
#define SYM_OP_LDY      0x05
#define SYM_OP_CPY      0x06
#define SYM_OP_CPX      0x07
#define SYM_OP_BIT      0x08
#define SYM_OP_STY      0x09
#define SYM_OP_PHP      0x0a
#define SYM_OP_PLP      0x0b
#define SYM_OP_PHA      0x0c
#define SYM_OP_PLA      0x0d
#define SYM_OP_DEY      0x0e
#define SYM_OP_TAY      0x0f
#define SYM_OP_INY      0x10
#define SYM_OP_INX      0x11
#define SYM_OP_JMP      0x12
#define SYM_OP_BPL      0x13
#define SYM_OP_BMI      0x14
#define SYM_OP_BVC      0x15
#define SYM_OP_BVS      0x16
#define SYM_OP_BCC      0x17
#define SYM_OP_BCS      0x18
#define SYM_OP_BNE      0x19
#define SYM_OP_BEQ      0x1a
#define SYM_OP_CLC      0x1b
#define SYM_OP_SEC      0x1c
#define SYM_OP_CLI      0x1d
#define SYM_OP_SEI      0x1e
#define SYM_OP_TYA      0x1f
#define SYM_OP_CLV      0x20
#define SYM_OP_CLD      0x21
#define SYM_OP_SED      0x22
#define SYM_OP_SHY      0x23
#define SYM_OP_ORA      0x24
#define SYM_OP_AND      0x25
#define SYM_OP_EOR      0x26
#define SYM_OP_ADC      0x27
#define SYM_OP_STA      0x28
#define SYM_OP_LDA      0x29
#define SYM_OP_CMP      0x2a
#define SYM_OP_SBC      0x2b
#define SYM_OP_STP      0x2c
#define SYM_OP_LDX      0x2d
#define SYM_OP_ASL      0x2e
#define SYM_OP_ROL      0x2f
#define SYM_OP_LSR      0x30
#define SYM_OP_ROR      0x31
#define SYM_OP_STX      0x32
#define SYM_OP_DEC      0x33
#define SYM_OP_INC      0x34
#define SYM_OP_TXA      0x35
#define SYM_OP_TAX      0x36
#define SYM_OP_DEX      0x37
#define SYM_OP_TXS      0x38
#define SYM_OP_TSX      0x39
#define SYM_OP_SHX      0x3a
#define SYM_OP_SLO      0x3b
#define SYM_OP_RLA      0x3c
#define SYM_OP_SRE      0x3d
#define SYM_OP_RRA      0x3e
#define SYM_OP_SAX      0x3f
#define SYM_OP_LAX      0x40
#define SYM_OP_DCP      0x41
#define SYM_OP_ISC      0x42
#define SYM_OP_ANC      0x43
#define SYM_OP_ALR      0x44
#define SYM_OP_ARR      0x45
#define SYM_OP_XAA      0x46
#define SYM_OP_AXS      0x47
#define SYM_OP_AHX      0x48
#define SYM_OP_TAS      0x49
#define SYM_OP_LAS      0x4a

typedef struct
{
    uint8_t     operation;
    uint8_t     addressingMode;
} SymInstr;

int ninJitDecodeSym(NinState* state, const SymInstr** sym, uint16_t* addr, uint32_t paddr);

#endif
