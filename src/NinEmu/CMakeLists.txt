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

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Gamepad REQUIRED)

if (APPLE OR WIN32)
    find_package(QtDeploy REQUIRED)
endif()

file(GLOB_RECURSE SOURCES "*.cpp")
if (APPLE)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/data/nin.icns")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/data/nin.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
elseif(WIN32)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/data/nin.rc")
endif()

# Compilation
add_executable(nin MACOSX_BUNDLE ${SOURCES})
target_include_directories(nin PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(nin OpenGL::GL OpenAL::AL libnin Qt5::Core Qt5::Widgets Qt5::Gamepad Threads::Threads)

if (UNIX AND NOT APPLE)
    target_link_options(nin PRIVATE -pthread)
elseif (APPLE)
    target_compile_definitions(nin PRIVATE GL_SILENCE_DEPRECATION=1)
    target_compile_options(nin PRIVATE "-Wno-unknown-pragmas")
    set_target_properties(nin
        PROPERTIES
        OUTPUT_NAME "Nin"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/data/nin.plist"
        MACOSX_BUNDLE_ICON_FILE "nin"
    )
elseif(WIN32)
    get_target_property(OPENAL_DLL OpenAL::AL IMPORTED_LOCATION)
    add_custom_command(
        TARGET nin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENAL_DLL}" "$<TARGET_FILE_DIR:nin>"
        COMMAND "${QT_DEPLOY}" "$<TARGET_FILE:nin>" "--no-compiler-runtime" "--no-opengl-sw" "--no-system-d3d-compiler"
        COMMENT "Copying DLLs"
        VERBATIM
    )
endif ()
