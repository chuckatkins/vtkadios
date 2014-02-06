find_package(ADIOS REQUIRED NOMPI)
include_directories(${ADIOS_INCLUDE_DIRS})
add_definitions(${ADIOS_CFLAGS})

set(vtkADIOSIO_SOURCES
  vtkIOADIOSModule.h

  ADIOSUtilities.h            ADIOSUtilities.cxx
  ADIOSUtilitiesVTK.h         ADIOSUtilitiesVTK.cxx
  ADIOSWriter.h               ADIOSWriter.cxx
  ADIOSWriter.txx             ADIOSWriterVTKDefine.cxx
                           ADIOSWriterVTKWrite.cxx

  vtkADIOSWriter.h            vtkADIOSWriter.cxx

  vtkADIOSImageDataWriter.h   vtkADIOSImageDataWriter.cxx
)
add_library(vtkADIOSIO ${vtkADIOSIO_SOURCES})
target_link_libraries(vtkADIOSIO ${ADIOS_LIBRARIES})
