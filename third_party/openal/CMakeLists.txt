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

include(FetchContent)
set(OPENAL_VERSION 1.20.1)

set(ALSOFT_UTILS                OFF)
set(ALSOFT_EXAMPLES             OFF)
set(ALSOFT_TESTS                OFF)
set(ALSOFT_CONFIG               OFF)
set(ALSOFT_HRTF_DEFS            OFF)
set(ALSOFT_AMBDEC_PRESETS       OFF)
set(ALSOFT_UPDATE_BUILD_VERSION OFF)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W")
endif()

FetchContent_Declare(
    fc_openal
    URL                 "https://github.com/kcat/openal-soft/archive/openal-soft-${OPENAL_VERSION}.tar.gz"
    URL_HASH            "SHA256=C32D10473457A8B545AAB50070FE84BE2B5B041E1F2099012777EE6BE0057C13"
    #INSTALL_DIR			"${CMAKE_BINARY_DIR}/third_party"
    #CMAKE_ARGS
    #    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    #    "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    #    "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
    #    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
    #    "-DALSOFT_UTILS=OFF"
)

FetchContent_MakeAvailable(fc_openal)
