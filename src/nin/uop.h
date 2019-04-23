#ifndef UOP_H
#define UOP_H

#define UOP_ADDR_IMM            0x00
#define UOP_ADDR_ABS            0x01
#define UOP_ADDR_ABS_NOREAD     0x02
#define UOP_ADDR_ABS_X          0x03
#define UOP_ADDR_ABS_X_NOREAD   0x04
#define UOP_ADDR_ABS_Y          0x05
#define UOP_ADDR_ABS_Y_NOREAD   0x06

#define UOP_P_SET       0x10
#define UOP_P_UNSET     0x11

#define UOP_MOV         0x20
#define UOP_MOV_NOFLAG  0x21
#define UOP_LOAD        0x22
#define UOP_STORE       0x23
#define UOP_CMP         0x24
#define UOP_ADD_REG     0x25
#define UOP_ADD_MEM     0x26

#define UOP_BRANCH_SET      0x30
#define UOP_BRANCH_UNSET    0x31

#define UOP_ORA         0x40
#define UOP_AND         0x41
#define UOP_EOR         0x42
#define UOP_ADC         0x43

#endif
