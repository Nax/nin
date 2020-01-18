/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <libnin/libnin.h> 

static const uint16_t kApuFrameCyclesNTSC[5] = { 7457, 14913, 22371, 29829, 37281 };
static const uint16_t kApuNoisePeriodNTSC[16] = { 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 };
static const uint16_t kApuDmcPeriodNTSC[16] = { 214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27 };

static const uint16_t kApuFrameCyclesPAL[5]  = { 8312, 16626, 24938, 33252, 41560 };
static const uint16_t kApuNoisePeriodPAL[16] = { 4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778 };
static const uint16_t kApuDmcPeriodPAL[16] = { 199, 177, 158, 149, 138, 118, 105, 99, 88, 74, 66, 59, 49, 39, 33, 25 };

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
        r->apuFrameCycles = kApuFrameCyclesNTSC;
        r->apuNoisePeriod = kApuNoisePeriodNTSC;
        r->apuDmcPeriod = kApuDmcPeriodNTSC;
        break;
    case NIN_REGION_PAL:
        r->clockRate = 1662607;
        r->frameCycles = 33248;
        r->frameDelay = 19997200;
        r->cycleExtraIncrement = 1;
        r->firstVisibleScanline = 1;
        r->vblank = 70;
        r->apuFrameCycles = kApuFrameCyclesPAL;
        r->apuNoisePeriod = kApuNoisePeriodPAL;
        r->apuDmcPeriod = kApuDmcPeriodPAL;
        break;
    }

    state->audio.setSourceFrequency(r->clockRate);
}
