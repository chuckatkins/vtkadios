/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSUnstructuredGridWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSUnstructuredGridWriter - Read ADIOS ADIOS files.
// .SECTION Description
// vtkADIOSUnstructuredGridWriter Reads vtkUnstructuredGrid objects using ADIOS

#ifndef __vtkADIOSUnstructuredGridWriter_h
#define __vtkADIOSUnstructuredGridWriter_h

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSWriter.h"

class vtkUnstructuredGrid;

//----------------------------------------------------------------------------

class VTKIOADIOS_EXPORT vtkADIOSUnstructuredGridWriter : public vtkADIOSWriter
{
public:
  static vtkADIOSUnstructuredGridWriter* New();
  vtkTypeMacro(vtkADIOSUnstructuredGridWriter,vtkADIOSWriter);

  virtual ~vtkADIOSUnstructuredGridWriter(void);

protected:
  vtkADIOSUnstructuredGridWriter(void);

  //virtual bool RequestInformation(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  //virtual bool RequestUpdateExtent(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  //virtual bool RequestUpdateTimeDependentInformation(vtkInformation *request,
  //  vtkInformationVector **input, vtkInformationVector *output);

  virtual int FillOutputPortInformation(int, vtkInformation*);

private:
  vtkADIOSUnstructuredGridWriter(const vtkADIOSUnstructuredGridWriter&);  // Not implemented.
  void operator=(const vtkADIOSUnstructuredGridWriter&);  // Not implemented.
};

#endif
