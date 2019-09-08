#include <libnin/libnin.h>

#define CYCLES_PER_SAMPLE           37
#define APU                         state->apu

static const uint8_t kTriangleSequence[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static const int8_t kPulseSequence[] = {
    0x02, /* 0 1 0 0 0 0 0 0 */
    0x06, /* 0 1 1 0 0 0 0 0 */
    0x1e, /* 0 1 1 1 1 0 0 0 */
    0xf9, /* 1 0 0 1 1 1 1 1 */
};

static const uint8_t kLengthCounterLookup[32] = {
    10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

static const uint16_t kNoisePeriod[16] = {
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

static const uint8_t kDMCPeriod[] = {
    214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27
};

uint8_t ninApuRegRead(NinState* state, uint16_t reg)
{
    uint8_t value;

    value = 0;
    switch (reg)
    {
    default:
        break;
    case 0x15:
        if (APU.pulse[0].enabled)   value |= 0x01;
        if (APU.pulse[1].enabled)   value |= 0x02;
        if (APU.triangle.enabled)   value |= 0x04;
        if (APU.noise.enabled)      value |= 0x08;
        if (APU.dmc.enabled)        value |= 0x10;
        break;
    }

    return value;
}

static void pulseUpdateTarget(NinState* state, unsigned c)
{
    NinChannelPulse* ch;
    uint16_t tmp;

    ch = &APU.pulse[c];
    tmp = ch->timerPeriod >> ch->sweepShift;
    if (ch->sweepNegate)
        ch->sweepTarget = ch->timerPeriod - tmp - (1 - c);
    else
        ch->sweepTarget = ch->timerPeriod + tmp;
}

static void loadEnvelope(NinEnvelope* ev, uint8_t value)
{
    ev->halt = !!(value & 0x20);
    ev->constant = !!(value & 0x10);
    ev->volume = value & 0xf;
    ev->start = 1;
}

void ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value)
{
    unsigned i = (reg >> 2) & 1;

    switch (reg)
    {
    case 0x00: // Pulse Envelope
    case 0x04:
        APU.pulse[i].duty = kPulseSequence[value >> 6];
        loadEnvelope(&APU.pulse[i].envelope, value);
        break;
    case 0x01: // Pulse Sweep
    case 0x05:
        APU.pulse[i].sweepEnable = !!(value & 0x80);
        APU.pulse[i].sweepPeriod = (value >> 4) & 0x07;
        APU.pulse[i].sweepNegate = !!(value & 0x08);
        APU.pulse[i].sweepShift = value & 0x07;
        APU.pulse[i].sweepReload = 1;
        pulseUpdateTarget(state, i);
        break;
    case 0x02: // Pulse Timer Lo
    case 0x06:
        APU.pulse[i].timerPeriod &= 0xff00;
        APU.pulse[i].timerPeriod |= value;
        pulseUpdateTarget(state, i);
        break;
    case 0x03: // Pulse Timer Hi
    case 0x07:
        APU.pulse[i].timerPeriod &= 0x00ff;
        APU.pulse[i].timerPeriod |= ((value & 0x7) << 8);
        APU.pulse[i].seqIndex = 0;
        if (APU.pulse[i].enabled)
            APU.pulse[i].length = kLengthCounterLookup[value >> 3];
        pulseUpdateTarget(state, i);
        APU.pulse[i].envelope.start = 1;
        break;
    case 0x08:
        if (value & 0x80)
            APU.triangle.control = 1;
        else
            APU.triangle.control = 0;
        APU.triangle.linearReload = value & 0x7f;
        break;
    case 0x09:
        break;
    case 0x0a: // Triangle Timer Lo
        APU.triangle.timerPeriod &= 0xff00;
        APU.triangle.timerPeriod |= value;
        break;
    case 0x0b: // Triangle Timer Hi
        APU.triangle.timerPeriod &= 0x00ff;
        APU.triangle.timerPeriod |= ((value & 0x7) << 8);
        if (APU.triangle.enabled) APU.triangle.length = kLengthCounterLookup[value >> 3];
        APU.triangle.linearReloadFlag = 1;
        break;
    case 0xc: // Noise Envelope
        loadEnvelope(&APU.noise.envelope, value);
        break;
    case 0xe: // Noise Timer
        APU.noise.mode = !!(value & 0x80);
        APU.noise.timerPeriod = kNoisePeriod[value & 0xf];
        break;
    case 0xf: // Noise Length
        if (APU.noise.enabled)
            APU.noise.length = kLengthCounterLookup[value >> 3];
        APU.noise.envelope.start = 1;
        break;
    case 0x10: // DMC Config
        APU.dmc.irqEnable = !!(value & 0x80);
        APU.dmc.loop = !!(value & 0x40);
        APU.dmc.timerPeriod = kDMCPeriod[value & 0xf];
        break;
    case 0x11: // DMC Load
        APU.dmc.output = value & 0x7f;
        break;
    case 0x12: // DMC addr
        APU.dmc.address = 0xc000 | ((uint16_t)value << 6);
        break;
    case 0x13: // DMC Length
        APU.dmc.length = ((uint16_t)value << 4) | 1;
        break;
    case 0x15: // STATUS
        if (value & 0x01)
            APU.pulse[0].enabled = 1;
        else
        {
            APU.pulse[0].enabled = 0;
            APU.pulse[0].length = 0;
        }
        if (value & 0x02)
            APU.pulse[1].enabled = 1;
        else
        {
            APU.pulse[1].enabled = 0;
            APU.pulse[1].length = 0;
        }
        if (value & 0x04)
        {
            APU.triangle.enabled = 1;
        }
        else
        {
            APU.triangle.enabled = 0;
            APU.triangle.length = 0;
        }
        if (value & 0x08)
        {
            APU.noise.enabled = 1;
        }
        else
        {
            APU.noise.enabled = 0;
            APU.noise.length = 0;
        }
        if (value & 0x10)
        {
            APU.dmc.enabled = 1;
        }
        else
        {
            APU.dmc.enabled = 0;
        }
        break;
    }
}

static int16_t ninMix(uint8_t triangle, uint8_t pulse1, uint8_t pulse2, uint8_t noise, uint8_t dmc)
{
    float fPulse;
    float fTND;

    //pulse1 = 0;
    //pulse2 = 0;
    //noise = 0;
    //triangle = 0;
    //dmc = 0;

    if (pulse1 || pulse2)
    {
        fPulse = 95.88f / ((8128.f / ((float)pulse1 + (float)pulse2)) + 100.f);
    }
    else
        fPulse = 0.f;

    if (triangle || noise || dmc)
    {
        fTND = ((float)triangle / 8227.f);
        fTND += ((float)noise / 12241.f);
        fTND += ((float)dmc / 22638.f);
        fTND = 159.79f / ((1.f / fTND) + 100.f);
    }
    else
        fTND = 0.f;

    return (int16_t)((fPulse + fTND) * 32767.f);
}

static void triangleClockQuarter(NinState* state)
{
    NinChannelTriangle* channel;

    channel = &APU.triangle;
    if (channel->linearReloadFlag)
        channel->linear = channel->linearReload;
    else if (channel->linear)
        channel->linear--;
    if (!channel->control)
        channel->linearReloadFlag = 0;
}

static void triangleClockHalf(NinState* state)
{
    if (APU.triangle.length && !APU.triangle.control)
        APU.triangle.length--;
}

static void triangleTick(NinState* state)
{
    NinChannelTriangle* channel;

    channel = &APU.triangle;
    if (channel->timerValue == 0)
    {
        channel->timerValue = channel->timerPeriod;
        if (channel->length && channel->linear)
        {
            channel->seqIndex++;
            channel->seqIndex &= 0x1f;
        }
    }
    else
    {
        channel->timerValue--;
    }
}

static void envelopeTick(NinEnvelope* ev)
{
    if (ev->start)
    {
        ev->start = 0;
        ev->decay = 15;
        ev->divider = ev->volume;
    }
    else
    {
        if (ev->divider)
            ev->divider--;
        else
        {
            ev->divider = ev->volume;
            if (ev->decay)
                ev->decay--;
            else if (ev->halt)
                ev->decay = 15;
        }
    }
}

static uint8_t sampleEnvelope(NinEnvelope* ev)
{
    if (ev->constant)
        return ev->volume;
    return ev->decay;
}

static void pulseClockHalf(NinState* state, unsigned c)
{
    NinChannelPulse* ch;

    ch = &APU.pulse[c];
    if (ch->sweepEnable && ch->sweepValue == 0 && ch->sweepTarget < 0x800)
    {
        ch->timerPeriod = ch->sweepTarget;
        pulseUpdateTarget(state, c);
    }
    if (ch->sweepValue == 0 || ch->sweepReload)
    {
        ch->sweepValue = ch->sweepPeriod;
        ch->sweepReload = 0;
    }
    else
    {
        ch->sweepValue--;
    }

    if (ch->length && !ch->envelope.halt)
        ch->length--;
}

static void pulseTick(NinState* state, unsigned c)
{
    NinChannelPulse* channel;

    channel = &APU.pulse[c];
    if (channel->timerValue == 0)
    {
        channel->timerValue = channel->timerPeriod;
        if (channel->length)
        {
            channel->seqIndex++;
            channel->seqIndex &= 0x07;
        }
    }
    else
    {
        channel->timerValue--;
    }
}

uint8_t samplePulse(NinState* state, unsigned c)
{
    NinChannelPulse* channel;

    channel = &APU.pulse[c];
    if (!channel->enabled || channel->timerValue < 8 || !channel->length || channel->sweepTarget >= 0x800)
        return 0;
    return (channel->duty & (1 << channel->seqIndex)) ? sampleEnvelope(&channel->envelope) : 0;
}

static void noiseClockHalf(NinState* state)
{
    NinChannelNoise* ch;

    ch = &APU.noise;

    if (ch->length && !ch->envelope.halt)
        ch->length--;
}

static void noiseTick(NinState* state)
{
    NinChannelNoise* ch;
    uint16_t tmp;
    uint16_t mask;

    ch = &APU.noise;
    if (ch->timerValue)
        ch->timerValue--;
    else
    {
        ch->timerValue = ch->timerPeriod;
        mask = ch->mode ? 0x40 : 0x02;
        tmp = (!!(ch->feedback & 0x01)) ^ (!!(ch->feedback & mask));
        ch->feedback >>= 1;
        ch->feedback |= (tmp << 14);
    }
}

static uint8_t sampleNoise(NinState* state)
{
    NinChannelNoise* ch;

    ch = &APU.noise;
    if (!ch->enabled || (ch->feedback & 0x01) || !ch->length)
        return 0;
    return sampleEnvelope(&ch->envelope);
}

static void dmcTick(NinState* state)
{
    NinChannelDMC* ch;
    uint8_t bit;

    ch = &APU.dmc;
    if (!ch->enabled)
        return;
    if (ch->timerValue)
    {
        ch->timerValue--;
        return;
    }
    ch->timerValue = ch->timerPeriod;
    if (ch->bitCount)
    {
        bit = ch->sampleBuffer & 0x01;
        ch->sampleBuffer >>= 1;
        ch->bitCount--;

        if (bit && ch->output <= 125)
            ch->output += 2;
        if (!bit && ch->output >= 2)
            ch->output -= 2;
    }
    if (!ch->bitCount && ch->length)
    {
        ch->length--;
        ch->sampleBuffer = ninMemoryRead8(state, ch->address | 0x8000);
        ch->bitCount = 8;
        ch->address++;
    }
}

void ninRunCyclesAPU(NinState* state, size_t cycles)
{
    uint8_t triangleSample;
    uint8_t pulseSample[2];
    uint8_t noiseSample;

    state->audioCycles += cycles;

    while (cycles--)
    {
        triangleTick(state);

        if (!APU.half)
        {
            pulseTick(state, 0);
            pulseTick(state, 1);
            noiseTick(state);
            dmcTick(state);

            switch (APU.frameCounter)
            {
            default:
                break;
            case 0:
            case 7456:
                triangleClockHalf(state);
                pulseClockHalf(state, 0);
                pulseClockHalf(state, 1);
                noiseClockHalf(state);
                /* fallthrough */
            case 3728:
            case 11185:
                triangleClockQuarter(state);
                envelopeTick(&APU.pulse[0].envelope);
                envelopeTick(&APU.pulse[1].envelope);
                envelopeTick(&APU.noise.envelope);
                break;
            }
            APU.frameCounter++;
            if (APU.frameCounter == 14914)
                APU.frameCounter = 0;
        }

        APU.half ^= 1;
    }

    if (state->audioCycles >= CYCLES_PER_SAMPLE)
    {
        /* Load the triangle sample */
        if (state->apu.triangle.enabled && state->apu.triangle.timerPeriod >= 2)
            triangleSample = kTriangleSequence[state->apu.triangle.seqIndex];
        else
            triangleSample = 0;
        pulseSample[0] = samplePulse(state, 0);
        pulseSample[1] = samplePulse(state, 1);
        noiseSample = sampleNoise(state);
        /* Emit the sample */
        state->audioCycles -= CYCLES_PER_SAMPLE;
        state->audioSamples[state->audioSamplesCount++] = ninMix(triangleSample, pulseSample[0], pulseSample[1], noiseSample, APU.dmc.output);
        if (state->audioSamplesCount == NIN_AUDIO_SAMPLE_SIZE)
        {
            if (state->audioCallback)
                state->audioCallback(state->audioCallbackArg, state->audioSamples);
            state->audioSamplesCount = 0;
        }
    }
}
