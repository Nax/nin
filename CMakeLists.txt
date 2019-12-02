cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(SUPERBUILD "Build this project using a superbuild" ON)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if (SUPERBUILD)
    project(SUPERBUILD)

    include(ExternalProject)

    add_subdirectory(third_party)

    ExternalProject_Add(
        nin
        SOURCE_DIR      "${CMAKE_SOURCE_DIR}"
        INSTALL_DIR     "${CMAKE_BINARY_DIR}"
        CMAKE_ARGS
            "-DSUPERBUILD=OFF"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
            "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/bin"
            "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DOPENAL_DIR=${CMAKE_BINARY_DIR}/third_party"
        DEPENDS
        openal_soft
    )
    ExternalProject_Add_Step(
		nin
		reconfigure
		COMMAND ${CMAKE_COMMAND} -E echo "Forcing a superbuild reconfigure"
		DEPENDEES download
		DEPENDERS configure
		ALWAYS ON
    )

    install(DIRECTORY "${CMAKE_BINARY_DIR}/bin/" DESTINATION . USE_SOURCE_PERMISSIONS)

    set(CPACK_PACKAGE_NAME "Nin")
    set(CPACK_PACKAGE_VENDOR "Maxime Bacoux")
    set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
    set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/pkg")

    if (WIN32)
        set(CPACK_GENERATOR "ZIP")
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_SYSTEM_NAME "mac")
    endif()

    include(CPack)
else()
    project(Nin VERSION "${VERSION}" LANGUAGES C CXX)

    set (CMAKE_CXX_STANDARD 17)

    if (NOT MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing -fno-exceptions -fno-rtti")
    else()
        string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc- /GR-")
    endif()

    add_subdirectory(src)
endif()
