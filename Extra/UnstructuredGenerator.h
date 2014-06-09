class vtkUnstructuredGrid;

void PillarGridGenerator(int, int wholeExtent[6], int extent[6], vtkUnstructuredGrid*);
void DiscontinuousGridGenerator(int, int wholeExtent[6], int extent[6], vtkUnstructuredGrid*);
void ContinuousGridGenerator(int, int wholeExtent[6], int extent[6], vtkUnstructuredGrid*);
void StepsGridGenerator(int, int wholeExtent[6], int extent[6], vtkUnstructuredGrid*, int);
void PyramidGridGenerator(int, int wholeExtent[6], int extent[6], vtkUnstructuredGrid*, int);
