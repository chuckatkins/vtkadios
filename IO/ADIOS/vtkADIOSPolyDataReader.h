/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSPolyDataReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSPolyDataReader - Read ADIOS ADIOS files.
// .SECTION Description
// vtkADIOSPolyDataReader Reads vtkPolyData objects using ADIOS

#ifndef __vtkADIOSPolyDataReader_h
#define __vtkADIOSPolyDataReader_h

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSReader.h"

class vtkPolyData;

//----------------------------------------------------------------------------

class VTKIOADIOS_EXPORT vtkADIOSPolyDataReader : public vtkADIOSReader
{
public:
  static vtkADIOSPolyDataReader* New();
  vtkTypeMacro(vtkADIOSPolyDataReader,vtkADIOSReader);

  // Retrieve the current output
  vtkPolyData* GetOutput(void);

  virtual ~vtkADIOSPolyDataReader(void);

protected:
  vtkADIOSPolyDataReader(void);

  //virtual bool RequestInformation(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  //virtual bool RequestUpdateExtent(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  //virtual bool RequestUpdateTime(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  //virtual bool RequestUpdateTimeDependentInformation(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  virtual bool RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);

  virtual int FillOutputPortInformation(int, vtkInformation*);

private:
  vtkADIOSPolyDataReader(const vtkADIOSPolyDataReader&);  // Not implemented.
  void operator=(const vtkADIOSPolyDataReader&);  // Not implemented.
};

#endif
