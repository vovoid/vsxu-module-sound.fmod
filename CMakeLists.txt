cmake_minimum_required(VERSION 2.8)
SET( CMAKE_MODULE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

# Get the Module Name from the file path
get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
set(module_id sound.fmod)
message("Configuring vsxu module            " ${module_id})

project (${module_id})

# Find the needed package first
# TODO: Make the GL Dependency optional by adding VSX_NO_GL definition if OpenGL is not found
find_package(VSXu REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

SET(ARCHITECTURE i386)
IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
  message("system is amd64")
  SET(ARCHITECTURE amd64)
  set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
ENDIF()


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib/win/api/inc/ )
  set(FMOD_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/api/lib/libfmodex.a")
  if (ARCHITECTURE STREQUAL "amd64")
    set(FMOD_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/api/lib/libfmodex64.a")
  endif()
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message("system is linux")
  if (ARCHITECTURE STREQUAL i386)
    message("configuring module for linux 32-bit")
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux32/api/inc/ )
    set(FMOD_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux32/api/lib/libfmodex.so")
    install(
      FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64/api/lib/libfmodex.so"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64/api/lib/libfmodex-4.38.00.so"
      DESTINATION
        ${VSXU_LIBRARY_DIRS}
    )
  endif()
  if (ARCHITECTURE STREQUAL amd64)
    message("configuring module for linux 64-bit")
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64/api/inc/ )
    set(FMOD_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64/api/lib/libfmodex64.so")
    install(
      FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64/api/lib/libfmodex64.so"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64/api/lib/libfmodex64-4.38.00.so"
      DESTINATION
        ${VSXU_LIBRARY_DIRS}
    )
  endif()
endif()





include_directories( ${VSXU_INCLUDE_DIRS} )


link_directories(${VSXu_LIBRARY_DIRS})





set(SOURCES main.cpp)
add_library(${module_id} MODULE ${SOURCES})

target_link_libraries(
  ${module_id}
  ${VSXU_LIBRARIES}
  ${GLEW_LIBRARY}
  ${OPENGL_LIBRARIES}
  ${FMOD_LIBRARY}
)

# Now Install the magic files
if(WIN32)
  target_link_libraries(${module_id} gdi32 ${VSXU_LIBRARIES})
  install(TARGETS ${module_id} DESTINATION ${VSXU_LIBRARY_DIRS}/plugins COMPONENT engine)
endif(WIN32)

if (UNIX)
    install(TARGETS ${module_id} DESTINATION ${VSXU_LIBRARY_DIRS}/vsxu/plugins)
endif(UNIX)

# Clean up the lib prefix from the module that would be installed
set_target_properties(
  ${module_id}
    PROPERTIES
      PREFIX ""
)

#Adding a nice shiny uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
