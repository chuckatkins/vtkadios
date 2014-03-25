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
// .NAME vtkADIOSWriter - Write ADIOS files.
// .SECTION Description
// vtkADIOSWriter is the base class for all ADIOS writers

#ifndef __vtkADIOSWriter_h
#define __vtkADIOSWriter_h

#include <string>

#include "vtkIOADIOSModule.h" // For export macro
#include "ADIOSWriter.h"
#include <vtkObject.h>

class vtkAbstractArray;
class vtkDataArray;
class vtkFieldData;
class vtkDataSet;
class vtkImageData;
class vtkPolyData;

class VTKIOADIOS_EXPORT vtkADIOSWriter : public vtkObject
{
public:
  static vtkADIOSWriter *New();
  vtkTypeMacro(vtkADIOSWriter,vtkObject);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

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
  // Define a VTK data type
  void Define(const std::string& path, const vtkAbstractArray* value);
  void Define(const std::string& path, const vtkDataArray* value);
  void Define(const std::string& path, const vtkFieldData* value);
  void Define(const std::string& path, const vtkDataSet* value);
  void Define(const std::string& path, const vtkImageData* value);
  void Define(const std::string& path, const vtkPolyData* value);

  // Description:
  // Open a file and prepare for writing already defined variables
  void InitializeFile(void);

  // Description:
  // Write a previously defined VTK data type
  void Write(const std::string& path, const vtkAbstractArray* value);
  void Write(const std::string& path, const vtkDataArray* value);
  void Write(const std::string& path, const vtkFieldData* value);
  void Write(const std::string& path, const vtkDataSet* value);
  void Write(const std::string& path, const vtkImageData* value);
  void Write(const std::string& path, const vtkPolyData* value);

protected:
  std::string FileName;
  ADIOSWriter Writer;

  vtkADIOSWriter();
  ~vtkADIOSWriter();

private:
  vtkADIOSWriter(const vtkADIOSWriter&);  // Not implemented.
  void operator=(const vtkADIOSWriter&);  // Not implemented.
};

#endif
