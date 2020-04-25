#ifndef LIBNIN_MAPPER_ID_H
#define LIBNIN_MAPPER_ID_H

namespace libnin
{

enum class MapperID
{
    Bad = -2,
    Unknown = -1,
    NROM = 0,
    MMC1,
    MMC2,
    MMC3,
    MMC4,
    MMC5,
    AxROM,
    AxROM_Conflicts,
    CNROM,
    GxROM,
    UxROM,
    UxROM_NoConflicts,
    UxROM_UN1ROM,
    UxROM_UNROM180,
    ColorDreams,
    Action52,
    FDS,
    MAX
};

MapperID getMapperID(int mapper, int submapper);

}

#endif
