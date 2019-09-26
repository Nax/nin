#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <cstdint>
#include <QMainWindow>
#include <QPointer>
#include <QGamepad>
#include <QString>

#include "RenderWidget.h"
#include "MemoryWindow.h"

class EmulatorWorker;
class Audio;
class MemoryWindow;
class AudioVisualizerWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    explicit MainWindow(QWidget* parent = nullptr);

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void openFile();
    void openRecentFile();

    void openWindowMemoryViewer();
    void openWindowAudioVisualizer();

private:
    void createActions();
    void createMenus();
    void updateRecentFiles();
    void addToRecentFiles(const QString& filename);
    void setupGamepad();

    void openRom(const QString& filename);

    enum { MaxRecentFiles = 9 };

    EmulatorWorker* _emu;
    Audio*          _audio;
    RenderWidget*   _render;

    QPointer<MemoryWindow>              _windowMemoryViewer;
    QPointer<AudioVisualizerWindow>     _windowAudioVisualizer;

    QGamepad*   _gamepad;

    QAction* _actionOpenFile;
    QAction* _actionOpenRecentFile[MaxRecentFiles];
    QAction* _actionExit;
    QAction* _pauseEmulation;
    QAction* _resumeEmulation;
    QAction* _actionMemoryViewer;
    QAction* _actionAudioVisualizer;

    QMenu* _fileMenu;
    QMenu* _emulationMenu;
    QMenu* _windowMenu;
};

#endif
