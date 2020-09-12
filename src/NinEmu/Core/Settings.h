/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
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

#ifndef NIN_EMU_CORE_SETTINGS_H
#define NIN_EMU_CORE_SETTINGS_H

#include <QObject>

enum class SettingAspectRatio
{
    Square = 0,
    Fit    = 1
};

enum class SettingOverscan
{
    None   = 0,
    Small  = 1,
    Medium = 2,
    Large  = 3
};

class Settings : public QObject
{
    Q_OBJECT;

public:
    explicit Settings(QObject* parent = nullptr);

    SettingAspectRatio aspectRatio() const;
    SettingOverscan    overscan() const;

    void setAspectRatio(SettingAspectRatio aspectRatio);
    void setOverscan(SettingOverscan overscan);

private:
    SettingAspectRatio _aspectRatio;
    SettingOverscan    _overscan;
};

#endif
