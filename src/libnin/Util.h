#ifndef LIBNIN_UTIL_H
#define LIBNIN_UTIL_H 1

#include <cstddef>

#define UNUSED(x)   ((void)x)

#if defined(WIN32) || defined(_WIN32)
# define UNREACHABLE() __assume(0)
#else
# define UNREACHABLE() __builtin_unreachable()
#endif

#endif
