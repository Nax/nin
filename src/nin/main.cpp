#include <QApplication>
#include "Emulator.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Emulator* emu;

    emu = new Emulator();
    if (argc > 1)
        emu->loadRom(argv[1]);
    emu->start();
    return app.exec();
}
