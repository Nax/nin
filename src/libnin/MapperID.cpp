#include <libnin/MapperID.h>

using namespace libnin;

MapperID libnin::getMapperID(int mapper, int submapper)
{
    switch (mapper)
    {
    case 0:
        return MapperID::NROM;
    case 1:
        return MapperID::MMC1;
    case 2:
        if (submapper == 1)
            return MapperID::UxROM_NoConflicts;
        return MapperID::UxROM;
    case 3:
        return MapperID::CNROM;
    case 4:
        return MapperID::MMC3;
    case 5:
        return MapperID::MMC5;
    case 7:
        if (submapper == 2)
            return MapperID::AxROM_Conflicts;
        return MapperID::AxROM;
    case 9:
        return MapperID::MMC2;
    case 10:
        return MapperID::MMC4;
    case 11:
        return MapperID::ColorDreams;
    case 20:
        return MapperID::FDS;
    case 66:
        return MapperID::GxROM;
    case 94:
        return MapperID::UxROM_UN1ROM;
    case 180:
        return MapperID::UxROM_UNROM180;
    case 228:
        return MapperID::Action52;
    default:
        return MapperID::Unknown;
    }
}
