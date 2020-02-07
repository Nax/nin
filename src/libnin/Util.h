#ifndef LIBNIN_UTIL_H
#define LIBNIN_UTIL_H 1

#define UNUSED(x)   ((void)x)

template <typename T>
inline static T* zalloc(size_t count)
{
    if (count)
        return new T[count]();
    return nullptr;
}

#endif
