project(ADIOS_VTK_Bridge)

cmake_minimum_required( VERSION 2.8 )

set(CMAKE_MODULE_PATH "${ADIOS_VTK_Bridge_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
if( NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ADIOS_VTK_Bridge_BINARY_DIR}/lib )
endif()
if( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ADIOS_VTK_Bridge_BINARY_DIR}/lib )
endif()
if( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ADIOS_VTK_Bridge_BINARY_DIR}/bin )
endif()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_subdirectory(IO)
add_subdirectory(Extra)

add_executable(ADIOSExodusII2ADIOS ADIOSExodusII2ADIOS.cxx)
target_link_libraries(ADIOSExodusII2ADIOS
  ${VTK_LIBRARIES} vtkIOADIOS ExtraFilters
)
add_executable(ADIOSBP2VTK ADIOSBP2VTK.cxx)
target_link_libraries(ADIOSBP2VTK
  ${VTK_LIBRARIES} vtkIOADIOS ExtraFilters
)
