#ifndef NIN_H
#define NIN_H

#include <stdio.h>
#include <stdint.h>

#if defined(__cplusplus)
# define NIN_API extern "C"
#else
# define NIN_API
#endif

typedef struct NinState_ NinState;

NIN_API NinState*       ninCreateState(FILE* rom);
NIN_API void            ninDestroyState(NinState* state);
NIN_API void            ninRunFrame(NinState* state);
NIN_API const uint32_t* ninGetScreenBuffer(NinState* state);

#endif
