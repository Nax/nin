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

NIN_API void ninAudioSetFrequencySource(NinState* state, uint32_t freq)
{
    NinAudio* audio = &state->audio;

    uint8_t windowBranchLen;
    double hyperbolicStep;
    double acc;
    float tmp;

    hyperbolicStep = 48000.0 / (double)freq;
    windowBranchLen = (uint8_t)round((double)DOWNSAMPLE_QUALITY / hyperbolicStep);
    acc = 0;
    for (uint8_t i = 1; i <= windowBranchLen; ++i)
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
    audio->dividerNum = 48000;
    audio->dividerDen = freq;
}

NIN_API void ninAudioSetCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg)
{
    NinAudio* audio = &state->audio;

    audio->callback = callback;
    audio->callbackArg = arg;
}

NIN_API void ninAudioPushSample(NinState* state, float sample)
{
    NinAudio* audio = &state->audio;
    size_t i;
    size_t j;
    float acc;

    /* We push samples into a ring buffer */
    audio->samplesSrc[audio->samplesCursorSrc++] = sample;
    if (audio->samplesCursorSrc == audio->windowSize)
        audio->samplesCursorSrc = 0;

    audio->dividerClock += audio->dividerNum;
    if (audio->dividerClock >= audio->dividerDen)
    {
        /* We need to generate a new sample */
        audio->dividerClock -= audio->dividerDen;
        acc = 0;
        for (i = 0; i < audio->windowSize; ++i)
        {
            j = i + audio->samplesCursorSrc;
            if (j >= audio->windowSize)
                j -= audio->windowSize;
            acc += audio->hyperbolicFactors[i] * audio->samplesSrc[j];
        }
        acc /= audio->windowSum;

        audio->samplesDst[audio->samplesCursorDst++] = (int16_t)(acc * 32000.f);
        // DEBUG
        //audio->samplesDst[audio->samplesCursorDst++] = (int16_t)(sample * 30000.f);
        if (audio->samplesCursorDst >= AUDIO_BUFFER_SIZE_TARGET)
        {
            audio->samplesCursorDst = 0;
            if (audio->callback)
                audio->callback(audio->callbackArg, audio->samplesDst);
        }
    }
}
