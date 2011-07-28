macro(init_ecto_kitchen)
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ecto)
  MESSAGE(FATAL_ERROR "Expecting ecto as a subdirectory of your toplevel project!")
endif()
set(ECTO_KITCHEN_VERSION 11.07.0)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
enable_testing()

add_subdirectory(ecto)
#this sets ecto_DIR so that subsequent projects may find ecto-config.cmake
set(ecto_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(KITCHEN_PROJECTS ${ARGN})
foreach(proj ${KITCHEN_PROJECTS})
  message(STATUS "~~ ${proj}")
  add_subdirectory(${proj})
endforeach()

#
# for kitchen testing
#
configure_file(ecto/kitchen/util/build_as_standalones.sh.in
  build_as_standalones.sh
  @ONLY)

add_subdirectory(ecto/kitchen/doc ${ecto_kitchen_BINARY_DIR}/doc)

string(REPLACE ";" " " KITCHEN_PROJECTS_STR "ecto;${KITCHEN_PROJECTS}")
configure_file(${ecto_kitchen_SOURCE_DIR}/ecto/kitchen/util/python_path.sh.in
  ${CMAKE_BINARY_DIR}/python_path.sh
  @ONLY
  )
message(STATUS "To set up your python path you may source the file 'python_path.sh'")
message(STATUS "  in the build directory.")

macro(check_hashbangs SCRIPTDIR)
  file(GLOB_RECURSE _python_scripts ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPTDIR}/*.py)
  foreach(file ${_python_scripts})
    add_custom_command(TARGET check_hashbangs
      COMMAND ${ecto_kitchen_SOURCE_DIR}/ecto/kitchen/util/check_python_script_hashbang.sh ${file}
      )
  endforeach()
endmacro()

add_custom_target(check_hashbangs)

foreach(proj ${KITCHEN_PROJECTS})
  check_hashbangs(${proj}/samples)
  check_hashbangs(${proj}/scripts)
endforeach()

add_test(check_hashbangs ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_BINARY_DIR} check_hashbangs)

endmacro()
