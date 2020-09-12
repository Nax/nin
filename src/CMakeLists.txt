# Nin, a Nintendo Entertainment System Emulator.
#
# Copyright (c) 2018-2020 Maxime Bacoux
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

# RPATH
set(CMAKE_MACOSX_RPATH      TRUE)
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_RPATH       "${CMAKE_BINARY_DIR}/third_party/lib")

if (UNIX)
    if (APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../lib")
    else()
        set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
    endif()
endif()

# C++
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

if (NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing -fno-exceptions -fno-rtti")
else()
    string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c- /GR-")
    add_definitions(-D_HAS_EXCEPTIONS=0 -D_CRT_SECURE_NO_WARNINGS=1)
endif()

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sized-deallocation")
endif()

if (NOT MSVC)
    add_compile_options("-Wall" "-Wextra")
endif()

if (WIN32)
    add_definitions(/D_WIN32_WINNT=0x0601)
endif()

add_subdirectory(libnin)
add_subdirectory(ninperf)
add_subdirectory(ninconv)
add_subdirectory(nintests)
add_subdirectory(NinEmu)

# Install
get_target_property(OPENAL_LIBRARY OpenAL::AL IMPORTED_LOCATION)
if (WIN32)
    install(TARGETS libnin RUNTIME DESTINATION .)
    install(TARGETS nin RUNTIME DESTINATION .)
    install(FILES "${OPENAL_LIBRARY}" DESTINATION .)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    include(InstallRequiredSystemLibraries)
    install(CODE "execute_process(COMMAND \"${QT_DEPLOY}\" \"--release\" \"\${CMAKE_INSTALL_PREFIX}/nin.exe\" \"--no-compiler-runtime\" \"--no-opengl-sw\" \"--no-system-d3d-compiler\")")
elseif (APPLE)
    install(TARGETS nin BUNDLE DESTINATION .)
    set(APP "\${CMAKE_INSTALL_PREFIX}/Nin.app")
    install(CODE "
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E make_directory \"${APP}/Contents/lib\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"${OPENAL_LIBRARY}\" \"${APP}/Contents/lib/\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"$<TARGET_FILE:libnin>\" \"${APP}/Contents/lib/\")
    execute_process(COMMAND \"${QT_DEPLOY}\" \"${APP}\" \"-verbose=1\")
    ")
else()
    include(GNUInstallDirs)
    install(
        TARGETS nin libnin
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
    install(FILES "${CMAKE_SOURCE_DIR}/data/nin.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(FILES "${CMAKE_SOURCE_DIR}/data/nin.png"     DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps")
endif()
