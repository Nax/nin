#ifndef LIBNIN_MEMBER_STATE_HELPER_H
#define LIBNIN_MEMBER_STATE_HELPER_H 1

namespace libnin
{

template <typename T>
struct MemberStateHelper
{
    using Pointer = MemberStateHelper<T>(T::*)();

    MemberStateHelper() : p{} {}
    MemberStateHelper(const MemberStateHelper<T>& h) : p{h.p} {}
    MemberStateHelper(Pointer pp) : p(pp) {}

    MemberStateHelper& operator=(const MemberStateHelper<T>& h) { p = h.p; return *this; }
    operator Pointer() { return p; }
    bool operator==(const MemberStateHelper<T>& other) const { return p == other.p; }

    Pointer p;
};

}

#endif
