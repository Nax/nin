find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(nintests ${SOURCES})
target_link_libraries(nintests libnin Threads::Threads)

if (UNIX AND NOT APPLE)
    target_link_options(nintests PRIVATE -pthread)
endif()

if (UNIX)
  set_target_properties(
      nintests
      PROPERTIES
      SKIP_BUILD_RPATH OFF
      BUILD_WITH_INSTALL_RPATH OFF
      INSTALL_RPATH_USE_LINK_PATH ON
  )
  if (APPLE)
    set_target_properties(nintests PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  else()
    set_target_properties(nintests PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
  endif()
endif()

install(TARGETS nintests RUNTIME DESTINATION tests)
install(DIRECTORY test_roms/ DESTINATION tests/roms)
