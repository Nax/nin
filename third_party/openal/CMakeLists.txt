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

include(ExternalProject)
set(OPENAL_VERSION 1.20.1)

set(OPENAL_INSTALL_DIR "${CMAKE_BINARY_DIR}/third_party")
set(OPENAL_INCLUDE_DIRECTORY "${OPENAL_INSTALL_DIR}/include")
set(OPENAL_LIBRARY_DIRECTORY "${OPENAL_INSTALL_DIR}/lib")

file(MAKE_DIRECTORY "${OPENAL_INCLUDE_DIRECTORY}")
file(MAKE_DIRECTORY "${OPENAL_LIBRARY_DIRECTORY}")

if (WIN32)
    set(OPENAL_IMPORTED_LOCATION    "${OPENAL_INSTALL_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}OpenAL32${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(OPENAL_IMPORTED_IMPLIB      "${OPENAL_INSTALL_DIR}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}OpenAL32${CMAKE_IMPORT_LIBRARY_SUFFIX}")
else()
    set(OPENAL_IMPORTED_LOCATION    "${OPENAL_INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}openal${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

ExternalProject_Add(
    ep_openal
    URL                 "https://github.com/kcat/openal-soft/archive/openal-soft-${OPENAL_VERSION}.tar.gz"
    URL_HASH            "SHA256=C32D10473457A8B545AAB50070FE84BE2B5B041E1F2099012777EE6BE0057C13"
    INSTALL_DIR			"${CMAKE_BINARY_DIR}/third_party"
    CMAKE_ARGS
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DALSOFT_UTILS=OFF"
        "-DALSOFT_EXAMPLES=OFF"
        "-DALSOFT_TESTS=OFF"
        "-DALSOFT_CONFIG=OFF"
        "-DALSOFT_HRTF_DEFS=OFF"
        "-DALSOFT_AMBDEC_PRESETS=OFF"
    BUILD_BYPRODUCTS
        ${OPENAL_IMPORTED_LOCATION}
        ${OPENAL_IMPORTED_IMPLIB}
)
if (APPLE)
    ExternalProject_Add_Step(
        ep_openal
        install_name
        DEPENDEES install
        COMMAND install_name_tool -id "@rpath/libopenal.dylib" "${OPENAL_IMPORTED_LOCATION}"
    )
endif()

add_library(OpenAL::AL SHARED IMPORTED GLOBAL)
set_target_properties(
    OpenAL::AL
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES   "${OPENAL_INCLUDE_DIRECTORY}"
    IMPORTED_LOCATION               "${OPENAL_IMPORTED_LOCATION}"
    IMPORTED_IMPLIB                 "${OPENAL_IMPORTED_IMPLIB}"
)
add_dependencies(OpenAL::AL ep_openal)
