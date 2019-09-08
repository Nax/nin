set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

message(STATUS "Prefix path: ${CMAKE_PREFIX_PATH}")

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(QtDeploy REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")
if (APPLE)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/data/nin.icns")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/data/nin.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
elseif(WIN32)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/data/nin.rc")
endif()

add_executable(nin WIN32 MACOSX_BUNDLE ${SOURCES})
target_link_libraries(nin OpenGL::GL OpenAL::AL libnin Qt5::Core Qt5::Widgets)

if (APPLE)
    target_compile_definitions(nin PRIVATE GL_SILENCE_DEPRECATION=1)
    target_compile_options(nin PRIVATE "-Wno-unknown-pragmas")
    set_target_properties(nin
        PROPERTIES
        OUTPUT_NAME "Nin"
        MACOSX_BUNDLE_ICON_FILE "nin"
    )
endif ()

install(TARGETS nin RUNTIME DESTINATION . BUNDLE DESTINATION .)

if (WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    include(InstallRequiredSystemLibraries)
    install(FILES "${OPENAL_DLL}" DESTINATION .)
    install(CODE "
    execute_process(COMMAND \"${QT_DEPLOY}\" \"--release\" \"${CMAKE_INSTALL_PREFIX}/nin.exe\" \"--no-compiler-runtime\" \"--no-opengl-sw\" \"--no-system-d3d-compiler\")
    ")
endif()

if (APPLE)
    install(CODE "
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"${OPENAL_LIBRARY}\" \"${CMAKE_INSTALL_PREFIX}/Nin.app/Contents/Frameworks/libopenal.1.dylib\")
    execute_process(COMMAND \"${QT_DEPLOY}\" \"${CMAKE_INSTALL_PREFIX}/Nin.app\" \"-libpath=${OPENAL_DIR}/lib\" \"-verbose=1\")
    ")
endif()
