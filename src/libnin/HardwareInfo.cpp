#include <libnin/HardwareInfo.h>

using namespace libnin;

HardwareInfo::HardwareInfo()
{
    setRegion(NIN_REGION_NTSC);
    setSystem(NIN_SYSTEM_NES);
}

void HardwareInfo::setRegion(NinRegion region)
{
    _region = region;
    switch (region)
    {
    case NIN_REGION_NTSC:
    default:
        _specs = HardwareSpecs::NTSC;
        break;
    case NIN_REGION_PAL:
        _specs = HardwareSpecs::PAL;
        break;
    }
}

void HardwareInfo::setSystem(NinSystem system)
{
    _system = system;
}
