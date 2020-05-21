#include <QtWidgets>
#include <NinEmu/Menu/DiskMenu.h>

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
