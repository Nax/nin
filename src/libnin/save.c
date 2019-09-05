#include <libnin/libnin.h>

NIN_API NinState* ninSetSaveFile(NinState* state, const char* path)
{
    FILE* f;

    f = fopen(path, "ab");
    if (f)
        fclose(f);

    f = fopen(path, "r+b");
    if (f)
    {
        fread(state->prgRam, state->prgRamSize, 1, f);
        state->saveFile = f;
    }
}

NIN_API NinState* ninSyncSave(NinState* state)
{
    FILE* f;

    f = state->saveFile;
    if (!f)
        return;

    fseek(f, 0, SEEK_SET);
    fwrite(state->prgRam, state->prgRamSize, 1, f);
    fflush(f);
}
