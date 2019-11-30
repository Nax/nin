#include <libnin/libnin.h>

static void mirroringSwitch(NinState* state)
{
    switch (state->mapperRegs.mmc1.mirroring)
    {
    case 0x00:
        ninMirrorA(state);
        break;
    case 0x01:
        ninMirrorB(state);
        break;
    case 0x02:
        ninMirrorH(state);
        break;
    case 0x03:
        ninMirrorV(state);
        break;
    }
}

static void bankSwitch(NinState* state)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;

    switch (mmc1->prgBankMode)
    {
    case 0:
    case 1:
        ninBankSwitchPrgRom16k(state, 0, (mmc1->prgBank & 0xfe) | 0);
        ninBankSwitchPrgRom16k(state, 1, (mmc1->prgBank & 0xfe) | 1);
        break;
    case 2:
        ninBankSwitchPrgRom16k(state, 0, 0);
        ninBankSwitchPrgRom16k(state, 1, mmc1->prgBank & 0xff);
        break;
    case 3:
        ninBankSwitchPrgRom16k(state, 0, mmc1->prgBank & 0xff);
        ninBankSwitchPrgRom16k(state, 1, state->prgBankCount - 1);
        break;
    }
}

static void bankSwitchChr(NinState* state)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;

    if (mmc1->chrBankMode)
    {
        ninBankSwitchChrRom4k(state, 0, mmc1->chrBank0);
        ninBankSwitchChrRom4k(state, 1, mmc1->chrBank1);
    }
    else
    {
        ninBankSwitchChrRom4k(state, 0, mmc1->chrBank0 & ~(0x01));
        ninBankSwitchChrRom4k(state, 1, mmc1->chrBank0 | 0x01);
    }
}

static int writeReg(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;

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
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;
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
