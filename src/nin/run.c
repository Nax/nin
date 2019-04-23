#include <nin/nin.h>

void ninRunFrame(NinState* state)
{
    ninSetFlagNMI(state, NMI_OCCURED);
    ninRunCycles(state, 2273);
    ninUnsetFlagNMI(state, NMI_OCCURED);
    ninRunCycles(state, 27507);
}
