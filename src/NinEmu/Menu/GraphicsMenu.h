#ifndef NIN_EMU_MENU_GRAPHICS
#define NIN_EMU_MENU_GRAPHICS

#include <QMenu>
#include <QAction>

#include <NinEmu/UX/AspectRatio.h>
#include <NinEmu/UX/Overscan.h>

class GraphicsMenu : public QMenu
{
    Q_OBJECT;
public:
    explicit GraphicsMenu(QWidget* parent = nullptr);

signals:
    void fullscreen(bool);
    void fit(bool);
    void integerScale(bool);
    void aspectRatio(AspectRatio);
    void overscan(Overscan);

private:
    void createSubMenuAspectRatio();
    void createSubMenuOverscan();

    void selectAspectRatio();
    void selectOverscan();
};

#endif
