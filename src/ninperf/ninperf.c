#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <nin/nin.h>

#if defined(WIN32) || defined(_WIN32)
# define LF "\r\n"
#else
# define LF "\n"
#endif

int main(int argc, char** argv)
{
    FILE* rom;
    NinState* state;
    clock_t lastFrame;
    clock_t now;
    int frameCount;

    if (argc != 2)
        return 1;

    state = ninCreateState(argv[1]);
    if (!state)
        return 1;

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
            printf("%d frames/s" LF, frameCount);
            fflush(stdout);
            frameCount = 0;
            lastFrame = now;
        }
    }

    return 0;
}
