find_package(ADIOS REQUIRED NOMPI)
include_directories(${ADIOS_INCLUDE_DIRS})
add_definitions(${ADIOS_CFLAGS})

set(vtkBPIO_SOURCES
  vtkIOBPModule.h

  BPUtilities.h            BPUtilities.cxx
  BPUtilitiesVTK.h         BPUtilitiesVTK.cxx
  BPWriter.h               BPWriter.cxx
  BPWriter.txx             BPWriterVTKDefine.cxx
                           BPWriterVTKWrite.cxx

  vtkBPWriter.h            vtkBPWriter.cxx

  vtkBPDataObjectWriter.h  vtkBPDataObjectWriter.cxx
  vtkBPDataSetWriter.h     vtkBPDataSetWriter.cxx
  vtkBPImageDataWriter.h   vtkBPImageDataWriter.cxx
)
add_library(vtkBPIO ${vtkBPIO_SOURCES})
target_link_libraries(vtkBPIO ${ADIOS_LIBRARIES})
