set(SOURCES ninperf.c)
add_executable(ninperf ${SOURCES})
target_link_libraries(ninperf libnin)
