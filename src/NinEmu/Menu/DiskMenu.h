#ifndef NIN_EMU_MENU_DISK
#define NIN_EMU_MENU_DISK

#include <vector>
#include <QMenu>
#include <QAction>

class DiskMenu : public QMenu
{
    Q_OBJECT;
public:
    explicit DiskMenu(QWidget* parent = nullptr);

    void setDiskSideCount(int diskCount);

signals:
    void insertDisk(int);

private slots:
    void selectDisk();

private:
    std::vector<QAction*>   _diskActions;
};

#endif
