#include <stdio.h>
#include <stdlib.h>
#include <libnin/libnin.h>

void ninApplyMapper(NinState* state, uint8_t mapperNum)
{
    switch (mapperNum)
    {
    default:
        printf("Unknown mapper number: %d\n", (int)mapperNum);
        exit(1);
    case 0:
        /* NROM */
        state->prgRomBank[0] = state->prgRom;
        if (state->prgRomSize > 0x4000)
            state->prgRomBank[1] = state->prgRom + 0x4000;
        else
            state->prgRomBank[1] = state->prgRom;
        break;
    }
}
