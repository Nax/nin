if (NOT MSVC)
    add_compile_options("-Wall" "-Wextra")
endif()

add_subdirectory(libnin)
add_subdirectory(nin)
add_subdirectory(ninperf)
