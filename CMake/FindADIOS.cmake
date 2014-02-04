# Find the ADIOS include files and library.
#
# ADIOS is a library for performing high-performance parallel IO operations.
# This module must be called with one of MPI or NOMPI required components.
#
# This module defines the following variables:
# ADIOS_INCLUDE_DIRS - where to find adios.h
# ADIOS_LIBRARIES    - the libraries to link against when using ADIOS
# ADIOS_CFLAGS       - the C / C++ flags to apply when using adios
# ADIOS_FOUND        - if false the library was not found.
#
# This module also defines the following macros:
# ADIOS_GENERATE


if(ADIOS_DIR)
  find_package(ADIOS NO_MODULE)
else()

  if((ADIOS_FIND_REQUIRED_MPI AND ADIOS_FIND_REQUIRED_NOMPI) OR (NOT ADIOS_FIND_REQUIRED_MPI AND NOT ADIOS_FIND_REQUIRED_NOMPI))
    message(SEND_ERROR "The ADIOS FIND module requires specifying either the MPI or NOMPI component")
  endif()

  # Backup the previous root path
  if(ADIOS_ROOT)
    set(_CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH})
    set(CMAKE_FIND_ROOT_PATH ${ADIOS_ROOT})
    set(_ADIOS_ROOT_OPTS ONLY_CMAKE_FIND_ROOT_PATH)
  endif()

  if(ADIOS_FIND_REQUIRED_NOMPI)
    set(_ADIOS_LIBNAME "adios_nompi")
  else() # ADIOS_FIND_REQUIRED_MPI
    set(_ADIOS_LIBNAME "adios")
  endif()

  find_path(ADIOS_INCLUDE_DIR adios.h           ${_ADIOS_ROOT_OPTS})
  find_library(ADIOS_LIBRARY  ${_ADIOS_LIBNAME} ${_ADIOS_ROOT_OPTS})

  # Restore the original root path
  if(ADIOS_ROOT)
    set(CMAKE_FIND_ROOT_PATH ${_CMAKE_FIND_ROOT_PATH})
  endif()
  
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADIOS ADIOS_INCLUDE_DIR ADIOS_LIBRARY)
  if(ADIOS_FOUND)
    set(ADIOS_INCLUDE_DIRS    ${ADIOS_INCLUDE_DIR})
    set(ADIOS_LIBRARIES   ${ADIOS_LIBRARY})
    if(ADIOS_FIND_REQUIRED_NOMPI)
      set(ADIOS_CFLAGS "-D_NOMPI")
    else()
      set(ADIOS_CFLAGS "")
    endif()
  endif()
endif()

