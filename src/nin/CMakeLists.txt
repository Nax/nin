set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenGL REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(nin ${SOURCES})
target_link_libraries(nin OpenGL::GL libnin Qt5::Widgets)
