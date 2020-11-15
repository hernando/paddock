# Taken from https://github.com/Eyescale/CMake/blob/master/CommonFindPackage.cmake

if(NOT PKGCONFIG_FOUND AND NOT MSVC)
  find_package(PkgConfig QUIET)
endif()
set(ENV{PKG_CONFIG_PATH}
  "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

macro(paddock_find_package Package_Name)
  string(TOUPPER ${Package_Name} PACKAGE_NAME)

  # Parse macro arguments
  set(__options QUIET REQUIRED SYSTEM)
  set(__oneValueArgs MODULE)
  set(__multiValueArgs)
  cmake_parse_arguments(
    __pkg "${__options}" "${__oneValueArgs}" "${__multiValueArgs}" ${ARGN})

  # Boost is always SYSTEM
  if(${Package_Name} STREQUAL "Boost")
    set(__pkg_SYSTEM TRUE)
  endif()

  # QUIET either via global option or macro argument
  set(__find_quiet)
  if(__pkg_QUIET)
    set(__find_quiet "QUIET")
  endif()

  # try standard cmake way
  find_package(${Package_Name} ${__find_quiet} ${__pkg_UNPARSED_ARGUMENTS})

  # if no results, try pkg_config way
  if((NOT ${Package_Name}_FOUND) AND (NOT ${PACKAGE_NAME}_FOUND) AND PKG_CONFIG_EXECUTABLE)
    # module name defaults to Package_Name if not provided
    if(NOT __pkg_MODULE)
      set(__pkg_MODULE ${Package_Name})
    endif()
    # Get (optional) version from arguments
    set(__package_version_check)
    if(__pkg_UNPARSED_ARGUMENTS)
      list(GET __pkg_UNPARSED_ARGUMENTS 0 __package_version)
      if(__package_version MATCHES "^[0-9.]+$") # is a version
        set(__package_version_check ">=${__package_version}")
      endif()
    endif()
    pkg_check_modules(${Package_Name} ${__pkg_MODULE}${__package_version_check}
      ${__find_quiet})
  endif()

  if(__pkg_REQUIRED)  # required find
    if(NOT (${Package_Name}_FOUND OR ${PACKAGE_NAME}_FOUND))
      message(FATAL_ERROR "Required ${Package_Name} not found")
    endif()
  endif()

  if(${PACKAGE_NAME}_FOUND)
    set(${Package_Name}_name ${PACKAGE_NAME})
    set(${Package_Name}_FOUND TRUE)
  elseif(${Package_Name}_FOUND)
    set(${Package_Name}_name ${Package_Name})
    set(${PACKAGE_NAME}_FOUND TRUE)
  else()
    # for common_find_package_post()
    list(APPEND ${PROJECT_NAME}_FIND_PACKAGES_NOTFOUND "${Package_Name}")
  endif()

  # for defines.h
  set(__use_package_define "PADDOCK_USE_${PACKAGE_NAME}")
  list(APPEND PADDOCK_FIND_PACKAGE_DEFINES ${__use_package_define})

endmacro()

macro(paddock_find_package_post)
  if(WIN32)
    set(__system Win32)
  endif()
  if(APPLE)
    set(__system Darwin)
  endif()
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(__system Linux)
  endif()

  list(APPEND PADDOCK_FIND_PACKAGE_DEFINES ${__system})

  # Write defines.h and options.cmake
  set(__options_cmake_file ${CMAKE_BINARY_DIR}/options.cmake)
  set(__defines_file ${CMAKE_BINARY_DIR}/include/paddock/defines.h)

  set(__defines_file_in ${__defines_file}.in)
  set(__options_cmake_file_in ${__options_cmake_file}.in)
  file(WRITE ${__defines_file_in}
    "// Automatically generated, do not edit.\n\n"
    "#ifndef PADDOCK_DEFINES_${__system}_H\n"
    "#define PADDOCK_DEFINES_${__system}_H\n\n")
  file(WRITE ${__options_cmake_file_in} "# Optional modules enabled during build\n")
  foreach(DEF ${PADDOCK_FIND_PACKAGE_DEFINES})
    string(REPLACE "-" "_" DEF ${DEF})
    string(REPLACE "+" "P" DEF ${DEF})
    add_definitions(-D${DEF}=1)
    file(APPEND ${__defines_file_in}
      "#ifndef ${DEF}\n"
      "#  define ${DEF} 1\n"
      "#endif\n")
    if(NOT DEF STREQUAL SYSTEM)
      file(APPEND ${__options_cmake_file_in} "set(${DEF} ON)\n")
    endif()
  endforeach()
  file(APPEND ${__defines_file_in} "\n#endif\n")

  # configure only touches file if changed, saves compilation after reconfigure
  configure_file(${__defines_file_in} ${__defines_file} COPYONLY)
  configure_file(${__options_cmake_file_in} ${__options_cmake_file} COPYONLY)

  include(${__options_cmake_file})
endmacro()