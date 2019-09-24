#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <cstdint>
#include <QMainWindow>
#include <QPointer>
#include <QGamepad>

#include "RenderWidget.h"
#include "MemoryWindow.h"

class EmulatorWorker;
class Audio;
class MemoryWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    explicit MainWindow(QWidget* parent = nullptr);

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void openRom();
    void openMemoryViewer();

private:
    void createActions();
    void createMenus();
    void setupGamepad();

    EmulatorWorker* _emu;
    Audio*          _audio;
    RenderWidget*   _render;

    QPointer<MemoryWindow> _memoryViewer;

    QGamepad*   _gamepad;

    QAction* _openRom;
    QAction* _pauseEmulation;
    QAction* _resumeEmulation;
    QAction* _actionMemoryViewer;

    QMenu* _fileMenu;
    QMenu* _emulationMenu;
    QMenu* _windowMenu;
};

#endif
