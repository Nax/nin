#ifndef NIN_EMULATOR_H
#define NIN_EMULATOR_H

#include <QTimer>
#include <nin/nin.h>
#include "NinMainWindow.h"

class NinEmulator : public QObject
{
    Q_OBJECT;

public:
    NinEmulator(const char* path);
    void start();

private slots:
    void update();

private:
    QTimer*         _timer;
    NinState*       _state;
    NinMainWindow*  _window;
};

#endif
