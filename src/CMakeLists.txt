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

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

if (NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing -fno-exceptions -fno-rtti")
else()
    string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c- /GR- /D_HAS_EXCEPTIONS=0")
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
add_subdirectory(NinEmu)
