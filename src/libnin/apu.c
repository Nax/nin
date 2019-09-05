#include <libnin/libnin.h>

#define CYCLES_PER_SAMPLE           37
#define TRIANGLE_VOLUME             300

static const int16_t kTriangleSequence[32] = {
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
        state->apu.pulseVolume[0] = value & 0x0f;
        state->apu.pulseHalt &= 0x02;
        state->apu.pulseHalt |= ((value >> 5) & 0x01);
        state->apu.pulseDuty[0] = (value >> 6) & 0x03;
        break;
    case 0x01: // Pulse 1 Sweep
        state->apu.pulseSweep[0].raw = value;
        state->apu.pulseSweepCounter[0] = state->apu.pulseSweep[0].divider;
        break;
    case 0x02: // Pulse 1 Timer Lo
        state->apu.pulseTimer[0] &= 0xff00;
        state->apu.pulseTimer[0] |= value;
        break;
    case 0x03: // Pulse 1 Timer Hi
        state->apu.pulseTimer[0] &= 0x00ff;
        state->apu.pulseTimer[0] |= ((value & 0x7) << 8);
        if (state->apu.pulseEnable & 0x01)
            state->apu.pulseLen[0] = kLengthCounterLookup[value >> 3];
        break;
    case 0x04: // Pulse 2 Envelope
        state->apu.pulseVolume[1] = value & 0x0f;
        state->apu.pulseHalt &= 0x01;
        state->apu.pulseHalt |= ((value >> 4) & 0x02);
        state->apu.pulseDuty[1] = (value >> 6) & 0x03;
        break;
    case 0x05: // Pulse 2 Sweep
        state->apu.pulseSweep[1].raw = value;
        state->apu.pulseSweepCounter[1] = state->apu.pulseSweep[1].divider;
        break;
    case 0x06: // Pulse 2 Timer Lo
        state->apu.pulseTimer[1] &= 0xff00;
        state->apu.pulseTimer[1] |= value;
        break;
    case 0x07: // Pulse 2 Timer Hi
        state->apu.pulseTimer[1] &= 0x00ff;
        state->apu.pulseTimer[1] |= ((value & 0x7) << 8);
        if (state->apu.pulseEnable & 0x02)
            state->apu.pulseLen[1] = kLengthCounterLookup[value >> 3];
        break;
    case 0x0a: // Triangle Timer Lo
        state->apu.triangleTimer &= 0xff00;
        state->apu.triangleTimer |= value;
        break;
    case 0x0b: // Triangle Timer Hi
        state->apu.triangleTimer &= 0x00ff;
        state->apu.triangleTimer |= ((value & 0x7) << 8);
        break;
    case 0x15: // STATUS
        state->apu.pulseEnable = (value & 0x03);
        if (!(value & 0x01))
            state->apu.pulseLen[0] = 0;
        if (!(value & 0x02))
            state->apu.pulseLen[1] = 0;
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

void ninRunCyclesAPU(NinState* state, size_t cycles)
{
    NinAPU apu;
    uint8_t triangleSample;
    uint8_t pulseSample[2];
    unsigned pulseSweepTarget;
    unsigned tmp;

    apu = state->apu;
    state->audioCycles += cycles;

    triangleSample = 0;
    pulseSample[0] = 0;
    pulseSample[1] = 0;

    while (cycles--)
    {
        triangleSample = 0;
        pulseSample[0] = 0;
        pulseSample[1] = 0;

        if (!apu.half)
        {
            apu.frameCounter++;
            if (apu.frameCounter == 7456 || apu.frameCounter == 14914)
            {
                for (int i = 0; i < 2; ++i)
                {
                    /* Sweep */
                    tmp = apu.pulseTimer[i] >> apu.pulseSweep[i].shift;
                    if (apu.pulseSweep[i].negate)
                        pulseSweepTarget = apu.pulseTimer[i] - tmp;
                    else
                        pulseSweepTarget = apu.pulseTimer[i] + tmp;

                    if (pulseSweepTarget > 0x7FF)
                        apu.pulseTimer[i] = 0;

                    if (apu.pulseSweep[i].enabled)
                    {
                        if (!apu.pulseSweepCounter[i])
                        {
                            apu.pulseTimer[i] = pulseSweepTarget;
                            apu.pulseSweepCounter[i] = apu.pulseSweep[i].divider;
                        }
                        else
                            apu.pulseSweepCounter[i]--;
                    }

                    if ((state->apu.pulseHalt & (1 << i)) && apu.pulseLen[i])
                        apu.pulseLen[i]--;
                }
                if (apu.frameCounter == 14914)
                    apu.frameCounter = 0;
            }

            for (int i = 0; i < 2; ++i)
            {
                if (apu.pulseLen[i] > 0)
                {
                    if (apu.pulseClock[i] == 0)
                    {
                        apu.pulseClock[i] = apu.pulseTimer[i];
                        apu.pulseSeq[i] = (apu.pulseSeq[i] + 1) % 8;
                    }
                    else
                        apu.pulseClock[i]--;
                }
            }
        }

        apu.half ^= 1;

        for (int i = 0; i < 2; ++i)
        {
            if (apu.pulseTimer[i] && apu.pulseLen[i] > 0)
            {
                tmp = (kPulseSequence[apu.pulseDuty[i]] >> apu.pulseSeq[i]) & 1;
                pulseSample[i] = tmp * apu.pulseVolume[i];
            }
        }

        if (apu.triangleTimer > 2)
        {
            if (apu.triangleClock == 0)
            {
                apu.triangleClock = apu.triangleTimer;
                apu.triangleSeq = (apu.triangleSeq + 1) % 32;
            }
            else
                apu.triangleClock--;
            triangleSample = kTriangleSequence[apu.triangleSeq];
        }
    }

    if (state->audioCycles >= CYCLES_PER_SAMPLE)
    {
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

    state->apu = apu;
}
