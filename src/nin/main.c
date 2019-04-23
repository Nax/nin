#include <nin/nin.h>

int main(int argc, char** argv)
{
    FILE* rom;
    NinState* state;

    if (argc != 2)
        return 1;

    rom = fopen(argv[1], "rb");
    state = ninCreateState(rom);
    fclose(rom);
    for (;;)
        ninRunFrame(state);
    ninDestroyState(state);
    return 0;
}
