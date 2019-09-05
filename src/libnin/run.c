#include <string.h>
#include <libnin/libnin.h>

void ninSetAudioCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg)
{
    state->audioCallback = callback;
    state->audioCallbackArg = arg;
}
