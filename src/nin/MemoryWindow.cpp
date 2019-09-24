#include <QVboxLayout>
#include "MemoryWindow.h"

MemoryWindow::MemoryWindow(QWidget* parent)
: QWidget(parent, Qt::Window)
{
    QVBoxLayout* layout;

    setWindowTitle("Memory Viewer");

    layout = new QVBoxLayout;
    setLayout(layout);

    _hexView = new HexView(0x10000);
    _hexView->show();
    _hexView->repaint();

    layout->addWidget(_hexView);
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
