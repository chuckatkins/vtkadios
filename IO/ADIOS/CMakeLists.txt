find_package(ADIOS REQUIRED NOMPI)
include_directories(${ADIOS_INCLUDE_DIRS})
add_definitions(${ADIOS_CFLAGS})

set(vtkADIOSIO_SOURCES
  vtkIOADIOSModule.h

  ADIOSUtilities.h            ADIOSUtilities.cxx
  ADIOSVarInfo.h              ADIOSVarInfo.cxx
  ADIOSReader.h               ADIOSReader.cxx
  ADIOSReaderImpl.h
  vtkADIOSDirTree.h           vtkADIOSDirTree.cxx
  vtkADIOSReader.h            vtkADIOSReader.cxx

  ADIOSWriter.h               ADIOSWriter.cxx
  vtkADIOSWriter.h            vtkADIOSWriter.cxx
  vtkADIOSWriterDefine.cxx    vtkADIOSWriterWrite.cxx
#
#  vtkADIOSImageDataWriter.h   vtkADIOSImageDataWriter.cxx
)
add_library(vtkADIOSIO ${vtkADIOSIO_SOURCES})
target_link_libraries(vtkADIOSIO ${ADIOS_LIBRARIES})
