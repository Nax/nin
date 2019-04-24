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

#define NIN_BUTTON_A        0x01
#define NIN_BUTTON_B        0x02
#define NIN_BUTTON_SELECT   0x04
#define NIN_BUTTON_START    0x08
#define NIN_BUTTON_UP       0x10
#define NIN_BUTTON_DOWN     0x20
#define NIN_BUTTON_LEFT     0x40
#define NIN_BUTTON_RIGHT    0x80

NIN_API NinState*       ninCreateState(FILE* rom);
NIN_API void            ninDestroyState(NinState* state);
NIN_API void            ninRunFrame(NinState* state);
NIN_API const uint32_t* ninGetScreenBuffer(NinState* state);
NIN_API void            ninSetInput(NinState* state, uint8_t input);

#endif
