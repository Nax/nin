#include <QApplication>
#include "NinEmulator.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    NinEmulator* emu;

    emu = new NinEmulator(argv[0]);
    return app.exec();
}
