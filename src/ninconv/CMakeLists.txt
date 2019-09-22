file(GLOB_RECURSE SOURCES "*.c")
add_executable(ninconv ${SOURCES})
#install(TARGETS ninconv RUNTIME DESTINATION .)

if(MSVC)
    target_compile_definitions(ninconv PRIVATE _CRT_SECURE_NO_WARNINGS=1)
endif()
