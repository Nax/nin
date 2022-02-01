#ifndef EMUSHELL_INCLUDED_NON_COPYABLE_H
#define EMUSHELL_INCLUDED_NON_COPYABLE_H

namespace EmuShell
{

class NonCopyable
{
public:
    NonCopyable() {}
    ~NonCopyable() {}
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}

#endif
