#include <libnin/libnin.h>

static void setBank(NinState* state, uint8_t slot, uint8_t number)
{
    state->prgRomBank[slot] = state->prgRom + 0x4000 * number;
}

static void setBankChr(NinState* state, uint8_t slot, uint8_t number)
{
    uint8_t* base;

    if (state->chrRam)
        base = state->chrRam;
    else
        base = state->chrRom;
    state->chrBank[slot] = base + 0x1000 * (number % state->chrBankCount);
}

static void mirroringSwitch(NinState* state)
{
    switch (state->mapper.mmc1.mirroring)
    {
    case 0x00:
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x000;
        state->nametables[2] = state->vram + 0x000;
        state->nametables[3] = state->vram + 0x000;
        break;
    case 0x01:
        state->nametables[0] = state->vram + 0x400;
        state->nametables[1] = state->vram + 0x400;
        state->nametables[2] = state->vram + 0x400;
        state->nametables[3] = state->vram + 0x400;
        break;
    case 0x02:
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x400;
        state->nametables[2] = state->vram + 0x000;
        state->nametables[3] = state->vram + 0x400;
        break;
    case 0x03:
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x000;
        state->nametables[2] = state->vram + 0x400;
        state->nametables[3] = state->vram + 0x400;
        break;
    }
}

static void bankSwitch(NinState* state)
{
    NinMapperRegsMMC1* mmc1 = &state->mapper.mmc1;

    switch (mmc1->prgBankMode)
    {
    case 0:
    case 1:
        setBank(state, 0, (mmc1->prgBank & 0xfe) | 0);
        setBank(state, 1, (mmc1->prgBank & 0xfe) | 1);
        break;
    case 2:
        setBank(state, 0, 0);
        setBank(state, 1, mmc1->prgBank & 0xff);
        break;
    case 3:
        setBank(state, 0, mmc1->prgBank & 0xff);
        setBank(state, 1, state->prgBankCount - 1);
        break;
    }
}

static void bankSwitchChr(NinState* state)
{
    NinMapperRegsMMC1* mmc1 = &state->mapper.mmc1;

    if (mmc1->chrBankMode)
    {
        setBankChr(state, 0, mmc1->chrBank0);
        setBankChr(state, 1, mmc1->chrBank1);
    }
    else
    {
        setBankChr(state, 0, mmc1->chrBank0 & ~(0x01));
        setBankChr(state, 1, mmc1->chrBank0 | 0x01);
    }
}

static int writeReg(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC1* mmc1 = &state->mapper.mmc1;

    switch ((addr & 0x7fff) >> 13)
    {
    case 0:
        /* 0x8000 - 0x9fff */
        mmc1->mirroring = value & 0x03;
        mmc1->prgBankMode = (value >> 2) & 0x03;
        mmc1->chrBankMode = (value >> 4) & 0x01;
        mirroringSwitch(state);
        bankSwitch(state);
        bankSwitchChr(state);
        return 1;
    case 1:
        /* 0xa000 - 0xbfff */
        mmc1->chrBank0 = value;
        bankSwitchChr(state);
        //getchar();
        return 0;
    case 2:
        /* 0xc000 - 0xdfff */
        mmc1->chrBank1 = value;
        bankSwitchChr(state);
        //getchar();
        return 0;
    case 3:
        /* 0xe000 - 0xffff */
        mmc1->prgBank = value & 0xf;
        bankSwitch(state);
        return 1;
    }
    return 1;
}

void ninPrgWriteHandlerMMC1(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC1* mmc1 = &state->mapper.mmc1;
    uint8_t shift;

    if (value & 0x80)
    {
        /* Reset the internal registers */
        mmc1->count = 0;
        mmc1->shift = 0;
        mmc1->prgBankMode = 3;
        bankSwitch(state);
        return;
    }

    shift = (((value & 1) << 4) | (mmc1->shift >> 1));
    if (mmc1->count < 4)
    {
        mmc1->shift = shift;
        mmc1->count++;
        return;
    }
    else
    {
        mmc1->shift = 0;
        mmc1->count = 0;
        writeReg(state, addr, shift);
    }

}
