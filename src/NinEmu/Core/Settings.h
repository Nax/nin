#ifndef NIN_EMU_CORE_SETTINGS_H
#define NIN_EMU_CORE_SETTINGS_H

#include <QObject>

enum class SettingAspectRatio
{
    Square = 0,
    Fit = 1
};

enum class SettingOverscan
{
    None = 0,
    Small = 1,
    Medium = 2,
    Large = 3
};

class Settings : public QObject
{
    Q_OBJECT;

public:
    explicit Settings(QObject* parent = nullptr);

    SettingAspectRatio  aspectRatio() const;
    SettingOverscan     overscan() const;

    void setAspectRatio(SettingAspectRatio aspectRatio);
    void setOverscan(SettingOverscan overscan);

private:
    SettingAspectRatio      _aspectRatio;
    SettingOverscan         _overscan;

};

#endif
