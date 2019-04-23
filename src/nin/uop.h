#ifndef UOP_H
#define UOP_H

#define UOP_ADDR_IMM            0x00
#define UOP_ADDR_ABS            0x01
#define UOP_ADDR_ABS_NOREAD     0x02

#define UOP_P_SET       0x10
#define UOP_P_UNSET     0x11

#define UOP_TAX         0x20
#define UOP_TAY         0x21
#define UOP_TSX         0x22
#define UOP_TXA         0x23
#define UOP_TXS         0x24
#define UOP_TYA         0x25

#define UOP_BRANCH_SET      0x30
#define UOP_BRANCH_UNSET    0x31

#define UOP_ORA         0x40
#define UOP_AND         0x41
#define UOP_EOR         0x42
#define UOP_ADC         0x43
#define UOP_STA         0x44
#define UOP_LDA         0x45
#define UOP_CMP         0x46

#define UOP_LDX         0x50
#define UOP_LDY         0x51

#endif
