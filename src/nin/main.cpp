#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Nin");
    QApplication::setOrganizationName("NotAndXor");

    MainWindow mainWindow;

    mainWindow.show();
    return app.exec();
}
