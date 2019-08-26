#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <nin/nin.h>

int main(int argc, char** argv)
{
    FILE* rom;
    NinState* state;
    clock_t lastFrame;
    clock_t now;
    int frameCount;

    if (argc != 2)
        return 1;

    rom = fopen(argv[1], "rb");
    if (!rom)
        return 1;

    state = ninCreateState(rom);
    fclose(rom);

    srand((unsigned)time(NULL));

    frameCount = 0;
    lastFrame = clock();
    for (;;)
    {
        ninSetInput(state, (rand() & 0xff));
        ninRunFrame(state);
        frameCount++;
        now = clock();
        if (now - lastFrame > CLOCKS_PER_SEC)
        {
            printf("%d frames/s\n", frameCount);
            frameCount = 0;
            lastFrame = now;
        }
    }

    return 0;
}
