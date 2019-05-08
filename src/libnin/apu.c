#include <libnin/libnin.h>

#define CYCLES_PER_SAMPLE           37
#define CYCLES_PER_LENGTH_COUNTER   7457

#define TRIANGLE_VOLUME             300

static const int16_t kTriangleSequence[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static const int16_t kPulseSequence[8] = {
    0, 1, 1, 1, 1, 0, 0, 0
};

uint8_t ninApuRegRead(NinState* state, uint16_t reg)
{
    uint8_t value;

    value = 0;
    switch (reg)
    {

    }

    return value;
}

void ninApuRegWrite(NinState* state, uint16_t reg, uint8_t value)
{
    switch (reg)
    {
    case 0x02: // Pulse 1 Timer Lo
        state->apu.pulseTimer[0] &= 0xff00;
        state->apu.pulseTimer[0] |= value;
        break;
    case 0x03: // Pulse 1 Timer Hi
        state->apu.pulseTimer[0] &= 0x00ff;
        state->apu.pulseTimer[0] |= ((value & 0x7) << 8);
        break;
    case 0x06: // Pulse 2 Timer Lo
        state->apu.pulseTimer[1] &= 0xff00;
        state->apu.pulseTimer[1] |= value;
        break;
    case 0x07: // Pulse 2 Timer Hi
        state->apu.pulseTimer[1] &= 0x00ff;
        state->apu.pulseTimer[1] |= ((value & 0x7) << 8);
        break;
    case 0x0a: // Triangle Timer Lo
        state->apu.triangleTimer &= 0xff00;
        state->apu.triangleTimer |= value;
        break;
    case 0x0b: // Triangle Timer Hi
        state->apu.triangleTimer &= 0x00ff;
        state->apu.triangleTimer |= ((value & 0x7) << 8);
        break;
    }
}

void ninRunCyclesAPU(NinState* state, size_t cycles)
{
    NinAPU apu;
    int16_t sample;
    int16_t triangleSample;
    int16_t pulseSample[2];

    apu = state->apu;
    state->audioCycles += cycles;

    triangleSample = 0;
    pulseSample[0] = 0;
    pulseSample[1] = 0;

    while (cycles--)
    {
        if (!apu.half)
        {
            for (int i = 0; i < 2; ++i)
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

        apu.half ^= 1;

        for (int i = 0; i < 2; ++i)
        {
            if (apu.pulseTimer[i])
            {
                pulseSample[i] = kPulseSequence[apu.pulseSeq[i]] * 15 * TRIANGLE_VOLUME - ((15 * TRIANGLE_VOLUME) / 2);
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
            triangleSample = kTriangleSequence[apu.triangleSeq] * TRIANGLE_VOLUME - ((15 * TRIANGLE_VOLUME) / 2);
        }
    }

    sample = triangleSample;
    sample += pulseSample[0];
    sample += pulseSample[1];

    /*
    if (sample)
        printf("Sample: 0x%04x\n", (uint16_t)sample);*/

    if (state->audioCycles >= CYCLES_PER_SAMPLE)
    {
        /* Emit the sample */
        state->audioCycles -= CYCLES_PER_SAMPLE;
        state->audioSamples[state->audioSamplesCount++] = sample;
        if (state->audioSamplesCount == NIN_AUDIO_SAMPLE_SIZE)
        {
            if (state->audioCallback)
                state->audioCallback(state->audioCallbackArg, state->audioSamples);
            state->audioSamplesCount = 0;
        }
    }

    state->apu = apu;
}
