/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPImageDataWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkBPImageDataWriter - Write ADIOS BP ImageData files.
// .SECTION Description
// vtkBPImageDataWriter writes the ADIOS BP file format.
// The standard extension for this writer's file format is "bp".

// .SECTION See Also
// vtkBPPImageDataWriter

#ifndef __vtkBPImageDataWriter_h
#define __vtkBPImageDataWriter_h

#include "vtkIOBPModule.h" // For export macro
#include "vtkBPDataSetWriter.h"
#include "BPWriter.h"

class vtkImageData;

class VTKIOBP_EXPORT vtkBPImageDataWriter : public vtkBPDataSetWriter
{
public:
  static vtkBPImageDataWriter* New();
  vtkTypeMacro(vtkBPImageDataWriter,vtkBPDataSetWriter);
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
  vtkBPImageDataWriter();
  ~vtkBPImageDataWriter();

private:
  vtkBPImageDataWriter(const vtkBPImageDataWriter&);  // Not implemented.
  void operator=(const vtkBPImageDataWriter&);  // Not implemented.
};

#endif
