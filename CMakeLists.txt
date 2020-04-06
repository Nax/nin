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

cmake_minimum_required(VERSION 3.14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 7)
set(VERSION_PATCH 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project(Nin VERSION "${VERSION}" LANGUAGES C CXX)

add_subdirectory(third_party)
add_subdirectory(src)

# CPack
set(CPACK_PACKAGE_NAME "Nin")
set(CPACK_PACKAGE_VENDOR "Maxime Bacoux")
set(CPACK_PACKAGE_CONTACT "Maxime Bacoux")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/pkg")

if (WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_SYSTEM_NAME "mac")
elseif(UNIX)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgcc1, libstdc++6, libopenal1, libgl1, libopengl0, libqt5core5a, libqt5gui5, libqt5widgets5, libqt5gamepad5")
    set(CPACK_GENERATOR "DEB")
endif()
include(CPack)
