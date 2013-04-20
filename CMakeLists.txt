cmake_minimum_required(VERSION 2.8)
SET( CMAKE_MODULE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

# Get the Module Name from the file path
get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})
message("Configuring vsxu module            " ${module_id})

project (${module_id})

# Find the needed package first
# TODO: Make the GL Dependency optional by adding VSX_NO_GL definition if OpenGL is not found
find_package(VSXu REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories( ${VSXU_INCLUDE_DIRS} )
link_directories(${VSXu_LIBRARY_DIRS})

set(SOURCES main.cpp)
add_library(${module_id} MODULE ${SOURCES})

target_link_libraries(
  ${module_id}
  ${VSXU_LIBRARIES}
  ${GLEW_LIBRARY}
  ${OPENGL_LIBRARIES}
)

#Now Install the magic  files
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
