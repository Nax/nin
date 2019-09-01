file(GLOB_RECURSE SOURCES "*.c" "*.cpp")
add_library(libnin STATIC ${SOURCES})
target_include_directories(libnin PRIVATE "${CMAKE_SOURCE_DIR}/src" PUBLIC "${CMAKE_SOURCE_DIR}/include")
set_target_properties(libnin PROPERTIES OUTPUT_NAME nin)
