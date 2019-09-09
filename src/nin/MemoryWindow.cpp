#include <QVboxLayout>
#include "MemoryWindow.h"
#include "Emulator.h"

MemoryWindow::MemoryWindow(Emulator& emu, QWidget* parent)
: QWidget(parent, Qt::Window)
, _emu(emu)
{
    QVBoxLayout* layout;

    setWindowTitle("Memory Viewer");

    layout = new QVBoxLayout;
    setLayout(layout);

    _hexView = new HexView(0x10000);
    _hexView->show();
    _hexView->repaint();

    layout->addWidget(_hexView);

    connect(&emu, SIGNAL(gameUpdate(NinState*)), this, SLOT(refresh(NinState*)));
}

MemoryWindow::~MemoryWindow()
{
    printf("TEST\n");
    fflush(stdout);
}

void MemoryWindow::refresh(NinState* state)
{
    ninDumpMemory(state, _hexView->buffer(), 0, 0x10000);
    _hexView->update();
}
