/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkUnstructuredGeneratorSource.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkUnstructuredGeneratorSource - Generators of unstructured
//       reservoir grids.

#ifndef __vtkUnstructuredGeneratorSource_h
#define __vtkUnstructuredGeneratorSource_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkUnstructuredGeneratorSource : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkUnstructuredGeneratorSource *New();
  vtkTypeMacro(vtkUnstructuredGeneratorSource, vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetMacro(GeneratorMode, int);
  vtkSetMacro(GeneratorMode, int);

  vtkGetVector6Macro(DataExtent, int);
  vtkSetVector6Macro(DataExtent, int);

  vtkGetMacro(PyramidStepSize, int);
  vtkSetMacro(PyramidStepSize, int);

  //BTX
  enum GeneratorType
    {
    GENERATOR_PILLAR = 0,
    GENERATOR_DISCONTINOUS = 1,
    GENERATOR_CONTINUOUS = 2,
    GENERATOR_STEPS = 3,
    GENERATOR_PYRAMID = 4
    };
  //ETX

protected:
  vtkUnstructuredGeneratorSource();
  ~vtkUnstructuredGeneratorSource();

  virtual int RequestInformation(vtkInformation*,
                                 vtkInformationVector**,
                                 vtkInformationVector*);

  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector**,
                          vtkInformationVector*);

  int GeneratorMode;
  int DataExtent[6];
  int PyramidStepSize;

  void AddTemoralData(double time, vtkUnstructuredGrid* grid);

  vtkUnstructuredGrid* Cache;

private:
  vtkUnstructuredGeneratorSource(const vtkUnstructuredGeneratorSource&);  // Not implemented.
  void operator=(const vtkUnstructuredGeneratorSource&);  // Not implemented.
};

#endif
