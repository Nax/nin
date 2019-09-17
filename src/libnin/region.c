#include <stdio.h>
#include <libnin/libnin.h> 

void ninRegionApply(NinState* state)
{
    NinRegionData* r = &state->regionData;

    switch (state->region)
    {
    case NIN_REGION_NTSC:
        r->clockRate = 1789773;
        r->frameCycles = 29781;
        r->frameDelay = 16639267;
        r->cycleExtraIncrement = 0;
        r->firstVisibleScanline = 0;
        r->vblank = 20;
        break;
    case NIN_REGION_PAL:
        r->clockRate = 1662607;
        r->frameCycles = 33248;
        r->frameDelay = 19997200;
        r->cycleExtraIncrement = 1;
        r->firstVisibleScanline = 1;
        r->vblank = 70;
        break;
    }

    ninAudioSetFrequencySource(state, r->clockRate);
}
