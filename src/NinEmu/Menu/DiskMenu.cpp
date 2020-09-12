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

#include <NinEmu/Menu/DiskMenu.h>
#include <QtWidgets>

DiskMenu::DiskMenu(QWidget* parent)
: QMenu(parent)
{
    setTitle("");
}

void DiskMenu::setDiskSideCount(int diskSideCount)
{
    QAction* action;

    if (diskSideCount == 0)
    {
        setTitle("");
        return;
    }

    setTitle(tr("Disk"));
    for (auto a : _diskActions)
        removeAction(a);
    for (auto a : _diskActions)
        a->deleteLater();
    _diskActions.clear();

    switch (diskSideCount)
    {
    case 1:
        action = new QAction("Insert Disk", this);
        _diskActions.push_back(action);
        break;
    case 2:
        action = new QAction("Insert Side A", this);
        _diskActions.push_back(action);
        action = new QAction("Insert Side B", this);
        _diskActions.push_back(action);
        break;
    default:
        for (int i = 0; i < diskSideCount; ++i)
        {
            action = new QAction(QString("Insert Disk " + QString::number(i / 2 + 1) + " Side " + (i & 1 ? "B" : "A")), this);
            _diskActions.push_back(action);
        }
    }

    for (int i = 0; i < diskSideCount; ++i)
    {
        _diskActions[i]->setData(i);
        connect(_diskActions[i], &QAction::triggered, this, &DiskMenu::selectDisk);
        addAction(_diskActions[i]);
    }
}

void DiskMenu::selectDisk()
{
    int diskSide = qobject_cast<QAction*>(sender())->data().toInt();
    insertDisk(diskSide);
}
