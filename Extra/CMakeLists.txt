add_library(ExtraFilters
  vtkMultiProcessControllerHelper.h
  vtkMultiProcessControllerHelper.cxx
  vtkCleanArrays.h
  vtkCleanArrays.cxx
  vtkCompositeDataToUnstructuredGridFilter.cxx
  vtkCompositeDataToUnstructuredGridFilter.h

  UnstructuredGenerator.h
  UnstructuredGenerator.cxx
  vtkUnstructuredGeneratorSource.h
  vtkUnstructuredGeneratorSource.cxx
)
target_link_libraries(ExtraFilters ${VTK_LIBRARIES})
