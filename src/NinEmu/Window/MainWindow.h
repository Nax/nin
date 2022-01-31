/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
 * All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 * 
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 * 
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 * 
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QGamepad>
#include <QMainWindow>
#include <QPointer>
#include <QString>
#include <cstdint>

#include <NinEmu/UI/RenderWidget.h>
#include <NinEmu/UX/AspectRatio.h>
#include <NinEmu/UX/Overscan.h>

class EmulatorWorker;
class Audio;
class MemoryWindow;
class MemorySearchWindow;
class AudioVisualizerWindow;
class DebuggerWindow;
class Settings;

class DiskMenu;
class GraphicsMenu;

struct EmulatorInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    explicit MainWindow(QWidget* parent = nullptr);

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void emulationReset(const EmulatorInfo& info);

    void openFile();
    void openRecentFile();

    void openWindowMemoryViewer();
    void openWindowMemorySearch();
    void openWindowAudioVisualizer();
    void openWindowDebugger();

private:
    void createActions();
    void createMenus();

    void updateRecentFiles();
    void addToRecentFiles(const QString& filename);
    void setupGamepad();

    void openRom(const QString& filename);

    void eventFullscreen(bool fullscreen);
    void eventFit(bool fit);
    void eventIntegerScale(bool integerScale);
    void eventAspectRatio(AspectRatio aspectRatio);
    void eventOverscan(Overscan overscan);

    enum
    {
        MaxRecentFiles = 9
    };

    Settings*       _settings;
    EmulatorWorker* _emu;
    Audio*          _audio;
    RenderWidget*   _render;

    QPointer<MemoryWindow>          _windowMemoryViewer;
    QPointer<MemorySearchWindow>    _windowMemorySearch;
    QPointer<AudioVisualizerWindow> _windowAudioVisualizer;
    QPointer<DebuggerWindow>        _windowDebugger;

    QGamepad* _gamepad;

    QAction* _actionOpenFile;
    QAction* _actionOpenRecentFile[MaxRecentFiles];
    QAction* _actionExit;
    QAction* _pauseEmulation;
    QAction* _resumeEmulation;
    QAction* _actionStepFrame;
    QAction* _actionStepSingle;
    QAction* _actionMemoryViewer;
    QAction* _actionMemorySearch;
    QAction* _actionAudioVisualizer;
    QAction* _actionDebugger;

    QMenu*        _fileMenu;
    QMenu*        _emulationMenu;
    QMenu*        _windowMenu;
    DiskMenu*     _diskMenu;
    GraphicsMenu* _graphicsMenu;
};

#endif
