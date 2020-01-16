#ifndef NIN_JIT_IL_H
#define NIN_JIT_IL_H 1

#include <stdint.h>
#include <libnin/flat_list.h>

typedef struct
{
    uint8_t op;
} NinJitIlInstr;

FLAT_LIST_DECL(NinJitIlList, ninJitIlList, NinJitIlInstr)

#endif
