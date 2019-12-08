#include <NinEmu/Core/Settings.h>

Settings::Settings(QObject* parent)
: QObject(parent)
, _aspectRatio(SettingAspectRatio::Square)
, _overscan(SettingOverscan::None)
{

}

SettingAspectRatio Settings::aspectRatio() const
{
    return _aspectRatio;
}

SettingOverscan Settings::overscan() const
{
    return _overscan;
}

void Settings::setAspectRatio(SettingAspectRatio aspectRatio)
{
    _aspectRatio = aspectRatio;
}

void Settings::setOverscan(SettingOverscan overscan)
{
    _overscan = overscan;
}
