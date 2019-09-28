#if defined(_MSC_VER)
# include <intrin.h>
#else
# include <x86intrin.h>
#endif

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

    QApplication app(argc, argv);
    QApplication::setApplicationName("Nin");
    QApplication::setOrganizationName("NotAndXor");

    MainWindow mainWindow;

    mainWindow.show();
    return app.exec();
}
