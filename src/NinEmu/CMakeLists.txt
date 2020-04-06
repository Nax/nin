# BSD 2 - Clause License
#
# Copyright(c) 2019, Maxime Bacoux
# All rights reserved.
#
# Redistributionand use in sourceand binary forms, with or without
# modification, are permitted provided that the following conditions are met :
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditionsand the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditionsand the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Gamepad REQUIRED)

if (APPLE OR WIN32)
    find_package(QtDeploy REQUIRED)
endif()

if (UNIX)
    if (APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../lib")
    else()
        set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
    endif()
endif()

file(GLOB_RECURSE SOURCES "*.cpp")
if (APPLE)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/data/nin.icns")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/data/nin.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
elseif(WIN32)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/data/nin.rc")
endif()

add_executable(nin WIN32 MACOSX_BUNDLE ${SOURCES})
target_include_directories(nin PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(nin OpenGL::GL OpenAL::AL libnin Qt5::Core Qt5::Widgets Qt5::Gamepad Threads::Threads)

if (UNIX AND NOT APPLE)
    target_link_options(nin PRIVATE -pthread)
endif()

if (APPLE)
    target_compile_definitions(nin PRIVATE GL_SILENCE_DEPRECATION=1)
    target_compile_options(nin PRIVATE "-Wno-unknown-pragmas")
    set_target_properties(nin
        PROPERTIES
        OUTPUT_NAME "Nin"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/data/nin.plist"
        MACOSX_BUNDLE_ICON_FILE "nin"
    )
endif ()

install(TARGETS nin RUNTIME DESTINATION bin BUNDLE DESTINATION bin)

if (WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "bin")
    include(InstallRequiredSystemLibraries)
    install(FILES "${OPENAL_DLL}" DESTINATION bin)
    install(CODE "
    execute_process(COMMAND \"${QT_DEPLOY}\" \"--release\" \"${CMAKE_INSTALL_PREFIX}/bin/nin.exe\" \"--no-compiler-runtime\" \"--no-opengl-sw\" \"--no-system-d3d-compiler\")
    ")
elseif (APPLE)
    # TODO: un-hardcode this
    set(APP "${CMAKE_INSTALL_PREFIX}/bin/Nin.app")
    install(CODE "
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E make_directory \"${APP}/Contents/lib\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"${OPENAL_LIBRARY}\" \"${APP}/Contents/lib/libopenal.1.dylib\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"$<TARGET_FILE:libnin>\" \"${APP}/Contents/lib/libnin.dylib\")
    execute_process(COMMAND \"${QT_DEPLOY}\" \"${APP}\" \"-verbose=1\")
    ")
else()
endif()
