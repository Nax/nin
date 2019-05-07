set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(nin ${SOURCES})
target_link_libraries(nin OpenGL::GL ${OPENAL_LIBRARY} libnin Qt5::Widgets)
target_include_directories(nin PRIVATE ${OPENAL_INCLUDE_DIR})
