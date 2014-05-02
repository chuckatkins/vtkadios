add_library(ExtraFilters
  vtkMultiProcessControllerHelper.h
  vtkMultiProcessControllerHelper.cxx
  vtkCleanArrays.h
  vtkCleanArrays.cxx
  vtkCompositeDataToUnstructuredGridFilter.cxx
  vtkCompositeDataToUnstructuredGridFilter.h
)
target_link_libraries(ExtraFilters ${VTK_LIBRARIES})
