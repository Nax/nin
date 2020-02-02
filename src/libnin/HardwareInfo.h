#ifndef LIBNIN_HARDWARE_INFO_H
#define LIBNIN_HARDWARE_INFO_H 1

#include <nin/nin.h>
#include <libnin/NonCopyable.h>
#include <libnin/HardwareSpecs.h>

class HardwareInfo : NonCopyable
{
public:
    HardwareInfo();

    NinRegion               region() const { return _region; }
    NinSystem               system() const { return _system; }
    const HardwareSpecs&    specs() const { return _specs; }

    void setRegion(NinRegion region);
    void setSystem(NinSystem system);

private:
    NinRegion       _region;
    NinSystem       _system;
    HardwareSpecs   _specs;
};

#endif
