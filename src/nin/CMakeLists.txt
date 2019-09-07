set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(nin WIN32 ${SOURCES})
target_link_libraries(nin OpenGL::GL ${OPENAL_LIBRARY} libnin Qt5::Core Qt5::Widgets)
target_include_directories(nin PRIVATE ${OPENAL_INCLUDE_DIR})

if (WIN32)

elseif (APPLE)
    target_compile_definitions(nin PRIVATE GL_SILENCE_DEPRECATION=1)
    target_compile_options(nin PRIVATE "-Wno-unknown-pragmas")
endif ()

install(TARGETS nin RUNTIME DESTINATION .)

if (WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS "C:/Windows/SysNative/openal32.dll")
    include(InstallRequiredSystemLibraries)
endif()

install(CODE "
    include(\"${CMAKE_SOURCE_DIR}/cmake/DeployQt.cmake\")
    DeployQt(\"\${CMAKE_INSTALL_PREFIX}/nin.exe\")
    include(BundleUtilities)
    fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/nin.exe\" \"\" \"\")
")
