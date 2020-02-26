#ifndef LIBNIN_NON_COPYABLE_H
#define LIBNIN_NON_COPYABLE_H 1

namespace libnin
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
