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

uint8_t ninApuRegRead(NinState* state, uint16_t reg)
{
    uint8_t value;

    value = 0;
    switch (reg)
    {
    case 0x15:
        //value = state->apu.pulseEnable;
        break;
    }

    return value;
}

void ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value)
{
    unsigned i = (reg >> 2) & 1;

    switch (reg)
    {
    case 0x00: // Pulse Envelope
    case 0x04:
        APU.pulse[i].envelope.volume = value & 0x0f;
        APU.pulse[i].envelope.constant = !!(value & 0x10);
        APU.pulse[i].envelope.halt = !!(value & 0x20);
        APU.pulse[i].duty = kPulseSequence[value >> 6];
        break;
    case 0x01: // Pulse Sweep
    case 0x05:
        APU.pulse[i].sweepEnable = !!(value & 0x80);
        APU.pulse[i].sweepPeriod = (value >> 4) & 0x07;
        APU.pulse[i].sweepNegate = !!(value & 0x08);
        APU.pulse[i].sweepShift = value & 0x07;
        APU.pulse[i].sweepReload = 1;
        break;
    case 0x02: // Pulse Timer Lo
    case 0x06:
        APU.pulse[i].timerPeriod &= 0xff00;
        APU.pulse[i].timerPeriod |= value;
        break;
    case 0x03: // Pulse Timer Hi
    case 0x07:
        APU.pulse[i].timerPeriod &= 0x00ff;
        APU.pulse[i].timerPeriod |= ((value & 0x7) << 8);
        APU.pulse[i].seqIndex = 0;
        if (APU.pulse[i].enabled)
            APU.pulse[i].length = kLengthCounterLookup[value >> 3];
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
    case 0x15: // STATUS
        if (value & 0x01)
            APU.pulse[0].enabled = 1;
        else
        {
            APU.pulse[0].enabled = 0;
            APU.pulse[0].enabled = 0;
        }
        if (value & 0x02)
            APU.pulse[1].enabled = 1;
        else
        {
            APU.pulse[1].enabled = 0;
            APU.pulse[1].enabled = 0;
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
        break;
    }
}

static int16_t ninMix(uint8_t triangle, uint8_t pulse1, uint8_t pulse2)
{
    float fPulse;
    float fTND;

    if (pulse1 || pulse2)
    {
        fPulse = 95.88f / ((8128.f / ((float)pulse1 + (float)pulse2)) + 100.f);
    }
    else
        fPulse = 0.f;

    if (triangle)
    {
        fTND = 159.79f / ((1.f / ((float)triangle / 8227.f)) + 100.f);
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
    if (!channel->enabled || channel->timerValue < 8)
        return 0;
    return (channel->duty & (1 << channel->seqIndex)) ? 15 : 0;
}

void ninRunCyclesAPU(NinState* state, size_t cycles)
{
    uint8_t triangleSample;
    uint8_t pulseSample[2];
    unsigned pulseSweepTarget;
    unsigned tmp;

    state->audioCycles += cycles;

    while (cycles--)
    {
        triangleTick(state);
        pulseSample[0] = 0;
        pulseSample[1] = 0;

        if (!APU.half)
        {
            pulseTick(state, 0);
            pulseTick(state, 1);
            switch (APU.frameCounter)
            {
            default:
                break;
            case 7456:
            case 14914:
                triangleClockHalf(state);
                /* fallthrough */
            case 3728:
            case 11185:
                triangleClockQuarter(state);
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
        /* Emit the sample */
        state->audioCycles -= CYCLES_PER_SAMPLE;
        state->audioSamples[state->audioSamplesCount++] = ninMix(triangleSample, pulseSample[0], pulseSample[1]);
        if (state->audioSamplesCount == NIN_AUDIO_SAMPLE_SIZE)
        {
            if (state->audioCallback)
                state->audioCallback(state->audioCallbackArg, state->audioSamples);
            state->audioSamplesCount = 0;
        }
    }
}
