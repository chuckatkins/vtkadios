find_package (ADIOS REQUIRED)
add_definitions (${ADIOS_COMPILE_FLAGS})  
include_directories (${ADIOS_INCLUDE_PATH})

find_package(MPI REQUIRED)

set(vtkADIOSIO_SOURCES
  vtkIOADIOSModule.h

  ADIOSDefs.h                 ADIOSDefs.cxx
  ADIOSUtilities.h            ADIOSUtilities.cxx

  ADIOSScalar.h               ADIOSScalar.cxx
  ADIOSVarInfo.h              ADIOSVarInfo.cxx
  ADIOSReader.h               ADIOSReader.cxx
  ADIOSReaderImpl.h

  vtkADIOSDirTree.h           vtkADIOSDirTree.cxx
  vtkADIOSReader.h            vtkADIOSReader.cxx
  #vtkADIOSImageDataReader.h   vtkADIOSImageDataReader.cxx
  #vtkADIOSPolyDataReader.h    vtkADIOSPolyDataReader.cxx

  ADIOSWriter.h               ADIOSWriter.cxx
  vtkADIOSWriter.h            vtkADIOSWriter.cxx
  vtkADIOSWriterDefine.cxx    vtkADIOSWriterWrite.cxx
)
add_library(vtkIOADIOS ${vtkADIOSIO_SOURCES})
target_link_libraries(vtkIOADIOS
 ${VTK_LIBRARIES} ${ADIOS_LIBRARIES}
 ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES}
)
