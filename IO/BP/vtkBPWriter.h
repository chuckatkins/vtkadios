/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkBPWriter - Write ADIOS BP files.
// .SECTION Description
// vtkBPWriter is the base class for all BP writers

#ifndef __vtkBPWriter_h
#define __vtkBPWriter_h

#include <string>

#include "vtkIOBPModule.h" // For export macro
#include "BPWriter.h"

#include <vtkObject.h>

class VTKIOBP_EXPORT vtkBPWriter : public vtkObject
{
public:
  vtkTypeMacro(vtkBPWriter,vtkObject);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Get the writer's input.
  vtkObject* GetInput() const;
  //ETX

  //BTX
  // Description:
  // Get/Set the output filename
  const std::string& GetFileName() const
  {
    return this->FileName;
  }
  void SetFileName(const std::string& filename)
  {
    this->FileName = filename;
  }
  //ETX
  
  // Description:
  // The main interface which triggers the writer to start.
  virtual void Write();

protected:
  std::string FileName;
  vtkObject* Input;

  BPWriter Writer;

  vtkBPWriter();
  ~vtkBPWriter();

private:
  vtkBPWriter(const vtkBPWriter&);  // Not implemented.
  void operator=(const vtkBPWriter&);  // Not implemented.
};

#endif
