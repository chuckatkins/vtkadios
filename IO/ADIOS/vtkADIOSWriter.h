/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSWriter - Write ADIOS ADIOS files.
// .SECTION Description
// vtkADIOSWriter is the base class for all ADIOS writers

#ifndef __vtkADIOSWriter_h
#define __vtkADIOSWriter_h

#include <string>

#include "vtkIOADIOSModule.h" // For export macro
#include "ADIOSWriter.h"

#include <vtkObject.h>

class VTKIOADIOS_EXPORT vtkADIOSWriter : public vtkObject
{
public:
  vtkTypeMacro(vtkADIOSWriter,vtkObject);
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

  ADIOSWriter Writer;

  vtkADIOSWriter();
  ~vtkADIOSWriter();

private:
  vtkADIOSWriter(const vtkADIOSWriter&);  // Not implemented.
  void operator=(const vtkADIOSWriter&);  // Not implemented.
};

#endif
