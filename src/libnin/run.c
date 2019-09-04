#include <string.h>
#include <libnin/libnin.h>

void ninRunFrame(NinState* state)
{
    ninRunFrameCPU(state);
}

void ninSetAudioCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg)
{
    state->audioCallback = callback;
    state->audioCallbackArg = arg;
}
