set(CONFIG_IN "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in")
set(CONFIG_OUT "${CMAKE_BINARY_DIR}/include/nin/config.h")
configure_file("${CONFIG_IN}" "${CONFIG_OUT}")

file(GLOB_RECURSE SOURCES "*.c" "*.cpp")
list(APPEND SOURCES "${CONFIG_OUT}")

add_library(libnin STATIC ${SOURCES})
target_include_directories(libnin PRIVATE "${CMAKE_SOURCE_DIR}/src" PUBLIC "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}/include")
set_target_properties(libnin PROPERTIES OUTPUT_NAME nin)
