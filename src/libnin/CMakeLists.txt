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

find_package(Ruby REQUIRED)

set(CONFIG_IN "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in")
set(CONFIG_OUT "${CMAKE_BINARY_DIR}/include/nin/config.h")
configure_file("${CONFIG_IN}" "${CONFIG_OUT}")

set(SOURCES_CODEGEN_CPU "${CMAKE_BINARY_DIR}/CPU_instr.cpp")
add_custom_command(
    OUTPUT "${SOURCES_CODEGEN_CPU}"
    COMMAND "${RUBY_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/gen_6502.rb" "${SOURCES_CODEGEN_CPU}"
    DEPENDS "${CMAKE_SOURCE_DIR}/tools/gen_6502.rb"
    COMMENT "Generating CPU_instr.cpp"
    VERBATIM
)

file(GLOB_RECURSE SOURCES "*.c" "*.cpp")
list(APPEND SOURCES "${CONFIG_OUT}" "${SOURCES_CODEGEN_CPU}")

add_library(libnin SHARED ${SOURCES})
target_include_directories(libnin PRIVATE "${CMAKE_SOURCE_DIR}/src" PUBLIC "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}/include")

if (WIN32)
    set_target_properties(libnin PROPERTIES OUTPUT_NAME libnin)
    target_compile_definitions(libnin PRIVATE NIN_DLL=1)
else()
    set_target_properties(libnin PROPERTIES OUTPUT_NAME nin)
endif()
