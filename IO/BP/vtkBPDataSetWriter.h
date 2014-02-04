/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPDataSetWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkBPDataSetWriter - Write ADIOS BP files.
// .SECTION Description
// vtkBPDataSetWriter writes the a vtkDataSet in the ADIOS BP file format.
// This is an abstract base class called by concrete child classes

#ifndef __vtkBPDataSetWriter_h
#define __vtkBPDataSetWriter_h

#include "vtkIOBPModule.h" // For export macro
#include "vtkBPDataObjectWriter.h"

class vtkDataSet;

class VTKIOBP_EXPORT vtkBPDataSetWriter : public vtkBPDataObjectWriter
{
public:
  vtkTypeMacro(vtkBPDataSetWriter,vtkBPWriter);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Get the writer's input.
  vtkDataSet* GetInput() const;
  //ETX

  // Description:
  // The main interface which triggers the writer to start.
  virtual void Write();

protected:
  vtkBPDataSetWriter();
  ~vtkBPDataSetWriter();

private:
  vtkBPDataSetWriter(const vtkBPDataSetWriter&);  // Not implemented.
  void operator=(const vtkBPDataSetWriter&);  // Not implemented.
};

#endif
