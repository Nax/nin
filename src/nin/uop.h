#ifndef UOP_H
#define UOP_H

#define ADDR_NONE                       0x00
#define ADDR_IMM                        0x01
#define ADDR_ABS                        0x02
#define ADDR_ABS_NOREAD                 0x03
#define ADDR_ABS_INDIRECT_NOREAD        0x04
#define ADDR_ABS_X                      0x05
#define ADDR_ABS_X_NOREAD               0x06
#define ADDR_ABS_Y                      0x07
#define ADDR_ABS_Y_NOREAD               0x08
#define ADDR_ZERO                       0x09
#define ADDR_ZERO_NOREAD                0x0a
#define ADDR_ZERO_X                     0x0b
#define ADDR_ZERO_X_NOREAD              0x0c
#define ADDR_ZERO_Y                     0x0d
#define ADDR_ZERO_Y_NOREAD              0x0e
#define ADDR_ZERO_X_INDIRECT            0x0f
#define ADDR_ZERO_X_INDIRECT_NOREAD     0x10
#define ADDR_ZERO_Y_INDIRECT            0x11
#define ADDR_ZERO_Y_INDIRECT_NOREAD     0x12

#define UOP_P_SET       0x20
#define UOP_P_UNSET     0x21
#define UOP_MOV         0x22
#define UOP_MOV_NOFLAG  0x23
#define UOP_LOAD        0x24
#define UOP_STORE       0x25
#define UOP_CMP         0x26
#define UOP_ADD_REG     0x27
#define UOP_ADD_MEM     0x28
#define UOP_TEST        0x29

#define UOP_BRANCH_SET      0x30
#define UOP_BRANCH_UNSET    0x31
#define UOP_JMP             0x32
#define UOP_JSR             0x33
#define UOP_RTS             0x34
#define UOP_RTI             0x35

#define UOP_ORA         0x40
#define UOP_AND         0x41
#define UOP_EOR         0x42
#define UOP_ADC         0x43

#define UOP_ASL         0x50
#define UOP_ASL_REG     0x51
#define UOP_ROL         0x52
#define UOP_ROL_REG     0x53
#define UOP_LSR         0x54
#define UOP_LSR_REG     0x55
#define UOP_ROR         0x56
#define UOP_ROR_REG     0x57

#define UOP_PUSHA       0x60
#define UOP_PUSHP       0x61
#define UOP_POPA        0x62
#define UOP_POPP        0x63

#endif
