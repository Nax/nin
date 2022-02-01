#ifndef EMUSHELL_INCLUDED_CORE_WIN32_ARCH_H
#define EMUSHELL_INCLUDED_CORE_WIN32_ARCH_H

#if !defined(_WINDOWS_H_) && !defined(APIENTRY)
# define WIN32_LEAN_AND_MEAN 1
# define VC_EXTRALEAN 1
# include <windows.h>
#endif

#endif
