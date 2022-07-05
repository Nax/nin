#if defined(_WIN32)
# include <windows.h>
# include <cstdlib>
#endif

extern "C" int EmuShell_main(int argc, char** argv);

#if defined(WIN32)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return EmuShell_main(__argc, __argv);
}
#else
int main(int argc, char** argv)
{
    return EmuShell_main(argc, argv);
}
#endif
