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


find_path(OPENAL_INCLUDE_DIR al.h
    HINTS "${OPENAL_DIR}"
    PATH_SUFFIXES include/AL include/OpenAL include AL OpenAL
    NO_DEFAULT_PATH
)

find_library(OPENAL_LIBRARY
    NAMES OpenAL al openal libopenal OpenAL32
    HINTS "${OPENAL_DIR}"
    PATH_SUFFIXES lib lib64
    PATHS
    ${OPENAL_SEARCH_PATH}
    NO_DEFAULT_PATH
)

if (WIN32)
    find_file(OPENAL_DLL
        NAMES OpenAL32.dll
        HINTS "${OPENAL_DIR}"
        PATH_SUFFIXES bin
        PATHS
        ${OPENAL_SEARCH_PATH}
        NO_DEFAULT_PATH
    )
endif()

add_library(OpenAL::AL SHARED IMPORTED)
set_target_properties(OpenAL::AL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}")

if (WIN32)
    set_target_properties(OpenAL::AL PROPERTIES IMPORTED_LOCATION "${OPENAL_DLL}" IMPORTED_IMPLIB "${OPENAL_LIBRARY}")
else()
    set_target_properties(OpenAL::AL PROPERTIES IMPORTED_LOCATION "${OPENAL_LIBRARY}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenAL DEFAULT_MSG OPENAL_LIBRARY OPENAL_INCLUDE_DIR)

mark_as_advanced(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
