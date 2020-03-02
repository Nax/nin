file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(nintests ${SOURCES})
target_link_libraries(nintests libnin)

install(TARGETS nintests RUNTIME DESTINATION tests)
install(DIRECTORY test_roms/ DESTINATION tests/roms)
