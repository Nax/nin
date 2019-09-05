#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <cstdint>
#include <QMainWindow>
#include "RenderWidget.h"

class Emulator;
class MainWindow : public QMainWindow
{
    Q_OBJECT;
public:
    explicit MainWindow(Emulator& emu, QWidget* parent = nullptr);
    ~MainWindow();

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;

    void updateTexture(const char* texture);

private slots:
    void openRom();

private:
    void createActions();
    void createMenus();

    Emulator&       _emu;
    RenderWidget*   _render;

    QAction* _openRom;
    QAction* _pauseEmulation;
    QAction* _resumeEmulation;

    QMenu* _fileMenu;
    QMenu* _emulationMenu;
};

#endif
