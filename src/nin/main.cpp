#include <QApplication>
#include "NinEmulator.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    NinEmulator* emu;

    emu = new NinEmulator(argv[1]);
    emu->start();
    return app.exec();
}
