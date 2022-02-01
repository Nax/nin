#ifndef EMUSHELL_MAIN_H
#define EMUSHELL_MAIN_H

#ifdef __cplusplus
extern "C" int EmuShell_main(int argc, char** argv);
#else
extern int EmuShell_main(int argc, char** argv);
#endif
#define main EmuShell_main

#endif /* EMUSHELL_MAIN_H */
