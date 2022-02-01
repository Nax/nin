# Nin, a Nintendo Entertainment System Emulator.
#
# Copyright (c) 2018-2022 Maxime Bacoux
# All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# Version 2, as published by the Free Software Foundation.
#
# Alternatively, this program can be licensed under a commercial license
# upon request.
#
# When using the program under the GNU General Public License Version 2 license,
# the following apply:
#
#  1. This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#  2. You should have received a copy of the GNU General Public License
#     along with this program; if not, write to the Free Software
#     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

# Sources filtering
file(GLOB_RECURSE SOURCES "*.cpp")
if (NOT WIN32)
    list(FILTER SOURCES EXCLUDE REGEX "Win32(/|\\\\)(.*)")
endif()
if (NOT APPLE)
    list(FILTER SOURCES EXCLUDE REGEX "MacOS(/|\\\\)(.*)")
endif()
if (NOT (UNIX AND NOT APPLE))
    list(FILTER SOURCES EXCLUDE REGEX "Linux(/|\\\\)(.*)")
endif()
if (NOT UNIX)
    list(FILTER SOURCES EXCLUDE REGEX "Unix(/|\\\\)(.*)")
endif()
list(FILTER SOURCES EXCLUDE REGEX "WinMain\\.cpp")

add_library(EmuShell STATIC ${SOURCES})
add_library(EmuShellMain STATIC main.cpp)
target_link_libraries(EmuShell PUBLIC EmuShellMain)

if (WIN32)
    target_compile_definitions(EmuShell PRIVATE WIN32_LEAN_AND_MEAN=1 VC_EXTRALEAN=1 UNICODE=1 _UNICODE=1)
    target_compile_definitions(EmuShellMain PRIVATE WIN32_LEAN_AND_MEAN=1 VC_EXTRALEAN=1 UNICODE=1 _UNICODE=1)
endif()
target_include_directories(EmuShell PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(EmuShellMain PUBLIC "${CMAKE_SOURCE_DIR}/src")
