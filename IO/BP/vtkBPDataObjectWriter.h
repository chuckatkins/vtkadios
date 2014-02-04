/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPDataObjectWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkBPDataObjectWriter - Write ADIOS BP files.
// .SECTION Description
// vtkBPDataObjectWriter writes the a vtkDataObject in the ADIOS BP file format.

#ifndef __vtkBPDataObjectWriter_h
#define __vtkBPDataObjectWriter_h

#include "vtkIOBPModule.h" // For export macro
#include "vtkBPWriter.h"

class vtkDataObject;

class VTKIOBP_EXPORT vtkBPDataObjectWriter : public vtkBPWriter
{
public:
  vtkTypeMacro(vtkBPDataObjectWriter,vtkBPWriter);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Get/Set the writer's input.
  vtkDataObject* GetInput() const;
  //ETX

  // Description:
  // The main interface which triggers the writer to start.
  virtual void Write();

protected:
  vtkBPDataObjectWriter();
  ~vtkBPDataObjectWriter();

private:
  vtkBPDataObjectWriter(const vtkBPDataObjectWriter&);  // Not implemented.
  void operator=(const vtkBPDataObjectWriter&);  // Not implemented.
};

#endif
