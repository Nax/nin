file(GLOB_RECURSE SOURCES "*.c" "*.h")
add_executable(nin ${SOURCES})
target_include_directories(nin PRIVATE "${CMAKE_SOURCE_DIR}/src")
install(TARGETS nin RUNTIME DESTINATION "bin")
