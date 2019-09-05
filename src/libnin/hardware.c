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
        state->prgBankCount = 0;
        state->prgRomBank[0] = state->prgRom;
        if (state->prgRomSize > 0x4000)
            state->prgRomBank[1] = state->prgRom + 0x4000;
        else
            state->prgRomBank[1] = state->prgRom;
        state->chrBankCount = 0;
        if (state->chrRam)
        {
            state->chrBank[0] = state->chrRam;
            state->chrBank[1] = state->chrRam + 0x1000;
        }
        else
        {
            state->chrBank[0] = state->chrRom;
            state->chrBank[1] = state->chrRom + 0x1000;
        }
        break;
    case 1:
        /* MMC1 */
        state->prgWriteHandler = &ninPrgWriteHandlerMMC1;
        state->prgBankCount = state->prgRomSize / 0x4000;
        state->prgRomBank[0] = state->prgRom;
        state->prgRomBank[1] = state->prgRom + (state->prgBankCount - 1) * 0x4000ll;

        if (state->chrRam)
        {
            state->chrBankCount = state->chrRamSize / 0x1000;
            state->chrBank[0] = state->chrRam;
            state->chrBank[1] = state->chrRam + 0x1000;
        }
        else
        {
            state->chrBankCount = state->chrRomSize / 0x1000;
            state->chrBank[0] = state->chrRom;
            state->chrBank[1] = state->chrRom + 0x1000;
        }

        state->mapper.mmc1.chrBank0 = 0;
        state->mapper.mmc1.chrBank1 = 1;
        break;
    }
}
