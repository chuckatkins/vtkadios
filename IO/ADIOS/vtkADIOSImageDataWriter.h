/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSImageDataWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSImageDataWriter - Write ImageData objects using ADIOS
// .SECTION Description
// vtkADIOSImageDataWriter writes a vtkImageData object using the ADIOS
// IO framework.  The standard extension for this writer's file format is "bp".

// .SECTION See Also
// vtkADIOSImageDataWriter

#ifndef __vtkADIOSImageDataWriter_h
#define __vtkADIOSImageDataWriter_h

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSWriter.h"

class vtkImageData;

class VTKIOADIOS_EXPORT vtkADIOSImageDataWriter : public vtkADIOSWriter
{
public:
  static vtkADIOSImageDataWriter* New();
  vtkTypeMacro(vtkADIOSImageDataWriter,vtkADIOSWriter);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Get/Set the writer's input.
  vtkImageData* GetInput() const;
  void SetInput(vtkImageData* input);
  //ETX

  // Description:
  // The main interface which triggers the writer to start.
  virtual void Write();

protected:
  vtkADIOSImageDataWriter();
  ~vtkADIOSImageDataWriter();

private:
  vtkADIOSImageDataWriter(const vtkADIOSImageDataWriter&);  // Not implemented.
  void operator=(const vtkADIOSImageDataWriter&);  // Not implemented.
};

#endif
