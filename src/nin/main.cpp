#include <QApplication>
#include "Emulator.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Emulator* emu;
    int ret;

    emu = new Emulator;
    if (argc > 1)
        emu->loadRom(argv[1]);
    emu->start();
    ret = app.exec();
    delete emu;
    return ret;
}
