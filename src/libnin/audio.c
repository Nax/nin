#include <math.h>
#include <libnin/libnin.h>


NIN_API void ninAudioSetFrequency(NinState* state, uint32_t freq)
{
    NinAudio* audio = &state->audio;

    audio->dividerNum = freq;
}

NIN_API void ninAudioSetFrequencySource(NinState* state, uint32_t freq)
{
    NinAudio* audio = &state->audio;

    audio->dividerDen = freq;
}

NIN_API void ninAudioSetCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg)
{
    NinAudio* audio = &state->audio;

    audio->callback = callback;
    audio->callbackArg = arg;
}

static double ninQualityLoPass(NinAudio* audio, double sample)
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

static double ninQualityHiPass(NinAudio* audio, double sample)
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
    sample = ninQualityLoPass(audio, sample);
    audio->dividerClock += audio->dividerNum;
    if (audio->dividerClock >= audio->dividerDen)
    {
        sample = ninQualityHiPass(audio, sample);
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
