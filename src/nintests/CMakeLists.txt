find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(nintests ${SOURCES})
target_link_libraries(nintests libnin Threads::Threads)

if (UNIX AND NOT APPLE)
    target_link_options(nintests PRIVATE -pthread)
endif()

add_custom_target(
    tests
    COMMAND nintests
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data/test_roms"
    VERBATIM
    USES_TERMINAL
)
