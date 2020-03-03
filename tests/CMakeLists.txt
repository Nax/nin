file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(nintests ${SOURCES})
target_link_libraries(nintests libnin)

if (APPLE)
  set_target_properties(
      nintests
      PROPERTIES
      SKIP_BUILD_RPATH OFF
      BUILD_WITH_INSTALL_RPATH OFF
      INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
      INSTALL_RPATH_USE_LINK_PATH ON
  )
endif()

install(TARGETS nintests RUNTIME DESTINATION tests)
install(DIRECTORY test_roms/ DESTINATION tests/roms)
