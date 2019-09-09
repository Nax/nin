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
    NinState* state;
    clock_t lastFrame;
    clock_t now;
    size_t cyc;

    if (argc != 2)
        return 1;

    state = ninCreateState(argv[1]);
    if (!state)
        return 1;

    srand((unsigned)time(NULL));

    cyc = 0;
    lastFrame = clock();
    for (;;)
    {
        ninSetInput(state, (rand() & 0xff));
        ninRunCycles(state, 5000);
        cyc += 5000;
        now = clock();
        if (now - lastFrame > CLOCKS_PER_SEC)
        {
            printf("%llu cycles/s" LF, (unsigned long long)cyc);
            fflush(stdout);
            cyc = 0;
            lastFrame = now;
        }
    }

    return 0;
}
