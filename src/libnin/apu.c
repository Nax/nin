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
        value = state->apu.pulseEnable;
        break;
    }

    return value;
}

void ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value)
{
    switch (reg)
    {
    case 0x00: // Pulse 1 Envelope
        APU.pulseVolume[0] = value & 0x0f;
        APU.pulseHalt &= 0x02;
        APU.pulseHalt |= ((value >> 5) & 0x01);
        APU.pulseDuty[0] = (value >> 6) & 0x03;
        break;
    case 0x01: // Pulse 1 Sweep
        APU.pulseSweep[0].raw = value;
        APU.pulseSweepCounter[0] = state->apu.pulseSweep[0].divider;
        break;
    case 0x02: // Pulse 1 Timer Lo
        APU.pulseTimer[0] &= 0xff00;
        APU.pulseTimer[0] |= value;
        break;
    case 0x03: // Pulse 1 Timer Hi
        APU.pulseTimer[0] &= 0x00ff;
        APU.pulseTimer[0] |= ((value & 0x7) << 8);
        if (APU.pulseEnable & 0x01)
            APU.pulseLen[0] = kLengthCounterLookup[value >> 3];
        break;
    case 0x04: // Pulse 2 Envelope
        APU.pulseVolume[1] = value & 0x0f;
        APU.pulseHalt &= 0x01;
        APU.pulseHalt |= ((value >> 4) & 0x02);
        APU.pulseDuty[1] = (value >> 6) & 0x03;
        break;
    case 0x05: // Pulse 2 Sweep
        APU.pulseSweep[1].raw = value;
        APU.pulseSweepCounter[1] = state->apu.pulseSweep[1].divider;
        break;
    case 0x06: // Pulse 2 Timer Lo
        APU.pulseTimer[1] &= 0xff00;
        APU.pulseTimer[1] |= value;
        break;
    case 0x07: // Pulse 2 Timer Hi
        APU.pulseTimer[1] &= 0x00ff;
        APU.pulseTimer[1] |= ((value & 0x7) << 8);
        if (APU.pulseEnable & 0x02)
            APU.pulseLen[1] = kLengthCounterLookup[value >> 3];
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
        APU.pulseEnable = (value & 0x03);
        if (!(value & 0x01))
            APU.pulseLen[0] = 0;
        if (!(value & 0x02))
            APU.pulseLen[1] = 0;
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
            if (APU.frameCounter == 7456 || APU.frameCounter == 14914)
            {
                for (int i = 0; i < 2; ++i)
                {
                    /* Sweep */
                    tmp = APU.pulseTimer[i] >> APU.pulseSweep[i].shift;
                    if (APU.pulseSweep[i].negate)
                        pulseSweepTarget = APU.pulseTimer[i] - tmp;
                    else
                        pulseSweepTarget = APU.pulseTimer[i] + tmp;

                    if (pulseSweepTarget > 0x7FF)
                        APU.pulseTimer[i] = 0;

                    if (APU.pulseSweep[i].enabled)
                    {
                        if (!APU.pulseSweepCounter[i])
                        {
                            APU.pulseTimer[i] = pulseSweepTarget;
                            APU.pulseSweepCounter[i] = APU.pulseSweep[i].divider;
                        }
                        else
                            APU.pulseSweepCounter[i]--;
                    }

                    if ((state->apu.pulseHalt & (1 << i)) && APU.pulseLen[i])
                        APU.pulseLen[i]--;
                }
                if (APU.frameCounter == 14914)
                    APU.frameCounter = 0;
            }

            for (int i = 0; i < 2; ++i)
            {
                if (APU.pulseLen[i] > 0)
                {
                    if (APU.pulseClock[i] == 0)
                    {
                        APU.pulseClock[i] = APU.pulseTimer[i];
                        APU.pulseSeq[i] = (APU.pulseSeq[i] + 1) % 8;
                    }
                    else
                        APU.pulseClock[i]--;
                }
            }
        }

        APU.half ^= 1;

        for (int i = 0; i < 2; ++i)
        {
            if (APU.pulseTimer[i] && APU.pulseLen[i] > 0)
            {
                tmp = (kPulseSequence[APU.pulseDuty[i]] >> APU.pulseSeq[i]) & 1;
                pulseSample[i] = tmp * APU.pulseVolume[i];
            }
        }
    }

    if (state->audioCycles >= CYCLES_PER_SAMPLE)
    {
        /* Load the triangle sample */
        if (state->apu.triangle.enabled && state->apu.triangle.timerPeriod >= 2)
            triangleSample = kTriangleSequence[state->apu.triangle.seqIndex];
        else
            triangleSample = 0;
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
