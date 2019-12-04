/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
class DebuggerWindow;
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
    void openWindowDebugger();

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
    QPointer<DebuggerWindow>            _windowDebugger;

    QGamepad*   _gamepad;

    QAction* _actionOpenFile;
    QAction* _actionOpenRecentFile[MaxRecentFiles];
    QAction* _actionExit;
    QAction* _pauseEmulation;
    QAction* _resumeEmulation;
    QAction* _actionStepFrame;
    QAction* _actionStepSingle;
    QAction* _actionMemoryViewer;
    QAction* _actionAudioVisualizer;
    QAction* _actionDebugger;

    QMenu* _fileMenu;
    QMenu* _emulationMenu;
    QMenu* _windowMenu;
};

#endif
