if (NOT MSVC)
    add_compile_options("-Wall" "-Wextra")
endif()

if (WIN32)
    add_definitions(/D_WIN32_WINNT=0x0601)
endif()

add_subdirectory(libnin)
add_subdirectory(libnindb)
add_subdirectory(nin)
add_subdirectory(ninperf)
