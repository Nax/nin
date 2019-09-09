set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(QtDeploy REQUIRED)

set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../lib")

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
    # TODO: un-hardcode this
    set(APP "${CMAKE_INSTALL_PREFIX}/Nin.app")
    install(CODE "
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E make_directory \"${APP}/Contents/lib\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"${OPENAL_LIBRARY}\" \"${APP}/Contents/lib/libopenal.1.dylib\")
    execute_process(COMMAND \"${QT_DEPLOY}\" \"${APP}\" \"-verbose=1\")
    ")
endif()
