#include <math.h>
#include <libnin/libnin.h>

#define DOWNSAMPLE_QUALITY 3

#ifndef PI
# define PI (3.141592653589793)
#endif

static double sinc(double x)
{
    return sin(x) / x;
}

static double hyperbolicFactor(double x)
{
    double pix;

    if (x < 0.0)
        x = -x;
    if (x > (double)DOWNSAMPLE_QUALITY)
        return 0.0;

    pix = PI * x;
    return sinc(pix) * sinc(pix / (double)DOWNSAMPLE_QUALITY);
}

static float lerp(float a, float b, float coeff)
{
    return (1.f - coeff) * a + coeff * b;
}

static float filterDC(NinState* state, float v)
{
    NinAudio* audio = &state->audio;
    float tmp;

    tmp = v - audio->dcMean;
    audio->dcMean = lerp(audio->dcMean, v, 4e-5f);

    return tmp;
}

static float ninAudioLoPass(NinState* state, float sample, float coeff, float gain)
{
    float newSample;
    float a1;
    float b0;

    a1 = coeff;
    b0 = gain * (1.f - a1);

    newSample = b0 * sample + a1 * state->audioSampleLoLast;
    state->audioSampleLoLast = newSample;

    return newSample;
}

static void ninAudioComputeCoefficients(NinState* state)
{
    NinAudio* audio = &state->audio;

    uint16_t windowBranchLen;
    double hyperbolicStep;
    double acc;
    float tmp;

    if (!audio->dividerNum || !audio->dividerDen)
        return;

    hyperbolicStep = (double)audio->dividerNum / (double)audio->dividerDen;
    windowBranchLen = (uint16_t)round((double)DOWNSAMPLE_QUALITY / hyperbolicStep);
    acc = 0;
    for (uint16_t i = 1; i <= windowBranchLen; ++i)
    {
        tmp = (float)hyperbolicFactor((double)i * hyperbolicStep);
        audio->hyperbolicFactors[windowBranchLen + i] = tmp;
        audio->hyperbolicFactors[windowBranchLen - i] = tmp;
        acc += tmp * 2;
    }
    audio->hyperbolicFactors[windowBranchLen] = 1.0f;
    acc += 1.0;

    audio->windowSize = windowBranchLen * 2 + 1;
    audio->windowSum = (float)acc;
}

NIN_API void ninAudioSetFrequency(NinState* state, uint32_t freq)
{
    NinAudio* audio = &state->audio;

    audio->dividerNum = freq;
    ninAudioComputeCoefficients(state);
}

NIN_API void ninAudioSetFrequencySource(NinState* state, uint32_t freq)
{
    NinAudio* audio = &state->audio;

    audio->dividerDen = freq;
    ninAudioComputeCoefficients(state);
}

NIN_API void ninAudioSetCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg)
{
    NinAudio* audio = &state->audio;

    audio->callback = callback;
    audio->callbackArg = arg;
}

static double newLoPass(NinAudio* audio, double sample)
{
    static const float kGain = 6.259598570e+07;

    audio->loPassX[0] = audio->loPassX[1];
    audio->loPassX[1] = audio->loPassX[2];
    audio->loPassX[2] = audio->loPassX[3];
    audio->loPassX[3] = audio->loPassX[4];
    audio->loPassX[4] = audio->loPassX[5];

    audio->loPassX[5] = sample / kGain;

    audio->loPassY[0] = audio->loPassY[1];
    audio->loPassY[1] = audio->loPassY[2];
    audio->loPassY[2] = audio->loPassY[3];
    audio->loPassY[3] = audio->loPassY[4];
    audio->loPassY[4] = audio->loPassY[5];

    audio->loPassY[5] = (
        (audio->loPassX[0] + audio->loPassX[5])
        + 5 * (audio->loPassX[1] + audio->loPassX[4])
        + 10 * (audio->loPassX[2] + audio->loPassX[3])
        + 0.8337693903 * audio->loPassY[0]
        - 4.3203944730 * audio->loPassY[1]
        + 8.9577480484 * audio->loPassY[2]
        - 9.2893615611 * audio->loPassY[3]
        + 4.8182380843 * audio->loPassY[4]
        );

    return audio->loPassY[5];
}

static double newHiPass(NinAudio* audio, double sample)
{
    static const float kGain = 1.000654713;

    audio->hiPassX[0] = audio->hiPassX[1];
    audio->hiPassX[1] = audio->hiPassX[2];
    audio->hiPassX[2] = audio->hiPassX[3];

    audio->hiPassX[3] = sample / kGain;

    audio->hiPassY[0] = audio->hiPassY[1];
    audio->hiPassY[1] = audio->hiPassY[2];
    audio->hiPassY[2] = audio->hiPassY[3];

    audio->hiPassY[3] = (
        (audio->hiPassX[3] - audio->hiPassX[0])
        + 3 * (audio->hiPassX[1] - audio->hiPassX[2])
        + 0.9986918594 * audio->hiPassY[0]
        - 2.9973828628 * audio->hiPassY[1]
        + 2.9986910031 * audio->hiPassY[2]
        );

    return audio->hiPassY[3];
}

NIN_API void ninAudioPushSample(NinState* state, float sample)
{
    NinAudio* audio = &state->audio;

    /* We push samples into a ring buffer */
    sample = newLoPass(audio, sample);
    audio->dividerClock += audio->dividerNum;
    if (audio->dividerClock >= audio->dividerDen)
    {
        sample = newHiPass(audio, sample);
        audio->dividerClock -= audio->dividerDen;
        audio->samplesDst[audio->samplesCursorDst++] = sample;
        if (audio->samplesCursorDst >= AUDIO_BUFFER_SIZE_TARGET)
        {
            audio->samplesCursorDst = 0;
            if (audio->callback)
                audio->callback(audio->callbackArg, audio->samplesDst);
        }
    }
}
