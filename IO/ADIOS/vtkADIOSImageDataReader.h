/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSImageDataReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSImageDataReader - Read ADIOS ADIOS files.
// .SECTION Description
// vtkADIOSImageDataReader Reads vtkImageData objects using ADIOS

#ifndef __vtkADIOSImageDataReader_h
#define __vtkADIOSImageDataReader_h

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSReader.h"

class vtkImageData;

//----------------------------------------------------------------------------

class VTKIOADIOS_EXPORT vtkADIOSImageDataReader : public vtkADIOSReader
{
public:
  static vtkADIOSImageDataReader* New();
  vtkTypeMacro(vtkADIOSImageDataReader,vtkADIOSReader);

  // Retrieve the current output
  vtkImageData* GetOutput(void);

  virtual ~vtkADIOSImageDataReader(void);

protected:
  vtkADIOSImageDataReader(void);

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
  vtkADIOSImageDataReader(const vtkADIOSImageDataReader&);  // Not implemented.
  void operator=(const vtkADIOSImageDataReader&);  // Not implemented.
};

#endif
