#if defined(_WIN32)
# include <windows.h>
#endif

extern "C" int EmuShell_main(int argc, char** argv);

#if defined(WIN32)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    // TODO
    return EmuShell_main(0, nullptr);
}
#else
int main(int argc, char** argv)
{
    return EmuShell_main(argc, argv);
}
#endif
