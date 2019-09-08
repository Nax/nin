find_path(OPENAL_INCLUDE_DIR al.h
    HINTS "${OPENAL_DIR}"
    PATH_SUFFIXES include/AL include/OpenAL include AL OpenAL
    NO_DEFAULT_PATH
)

find_library(OPENAL_LIBRARY
    NAMES OpenAL al openal libopenal OpenAL32
    HINTS "${OPENAL_DIR}"
    PATH_SUFFIXES lib lib64
    PATHS
    ${OPENAL_SEARCH_PATH}
    NO_DEFAULT_PATH
)

if (WIN32)
    find_file(OPENAL_DLL
        NAMES OpenAL32.dll
        HINTS "${OPENAL_DIR}"
        PATH_SUFFIXES bin
        PATHS
        ${OPENAL_SEARCH_PATH}
        NO_DEFAULT_PATH
    )
endif()

add_library(OpenAL::AL SHARED IMPORTED)
set_target_properties(OpenAL::AL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}")

if (WIN32)
    set_target_properties(OpenAL::AL PROPERTIES IMPORTED_LOCATION "${OPENAL_DLL}" IMPORTED_IMPLIB "${OPENAL_LIBRARY}")
else()
    set_target_properties(OpenAL::AL PROPERTIES IMPORTED_LOCATION "${OPENAL_LIBRARY}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenAL DEFAULT_MSG OPENAL_LIBRARY OPENAL_INCLUDE_DIR)

mark_as_advanced(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
