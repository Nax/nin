#include <stdio.h>
#include <stdlib.h>
#include <libnin/libnin.h>

#define zalloc(n)   (calloc(1, n))

void ninApplyMapper(NinState* state, uint8_t mapperNum)
{
    state->prgWriteHandler = &ninPrgWriteHandlerNull;

    switch (mapperNum)
    {
    default:
        printf("Unknown mapper number: %d\n", (int)mapperNum);
        exit(1);
    case 0:
        /* NROM */
        state->bankCount = 0;
        state->prgRomBank[0] = state->prgRom;
        if (state->prgRomSize > 0x4000)
            state->prgRomBank[1] = state->prgRom + 0x4000;
        else
            state->prgRomBank[1] = state->prgRom;
        break;
    case 1:
        /* MMC1 */
        state->prgWriteHandler = &ninPrgWriteHandlerMMC1;
        state->bankCount = state->prgRomSize / 0x4000;
        state->prgRomBank[0] = state->prgRom;
        state->prgRomBank[1] = state->prgRom + (state->bankCount - 1) * 0x4000ll;
        state->prgRamSize = 0x2000;
        state->prgRam = zalloc(state->prgRamSize);
        break;
    }
}
