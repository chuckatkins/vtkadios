/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSReader - Read ADIOS ADIOS files.
// .SECTION Description
// vtkADIOSReader is the base class for all ADIOS writers

#ifndef __vtkADIOSReader_h
#define __vtkADIOSReader_h

#include <string>

#include "vtkIOADIOSModule.h" // For export macro
#include "ADIOSReader.h"
#include <vtkObject.h>

class vtkDataArray;
class vtkFieldData;
class vtkImageData;
class vtkPolyData;

class VTKIOADIOS_EXPORT vtkADIOSReader : public vtkObject
{
public:
  static vtkADIOSReader* New();
  vtkTypeMacro(vtkADIOSReader,vtkObject);
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
  // The main interface which triggers the reader to start
  virtual void Read();

  // Description:
  // Initialize a VTK object with it's scalar values and allocate any arrays
  // needing to be filled
  void InitializeObject(const std::string& path, vtkDataArray* data);
  void InitializeObject(const std::string& path, vtkFieldData* data);
  void InitializeObject(const std::string& path, vtkImageData* data);
  void InitializeObject(const std::string& path, vtkPolyData* data);
  

protected:
  

  std::string FileName;
  ADIOSReader Reader;

  struct vtkADIOSReaderImpl;
  vtkADIOSReaderImpl *Impl;

  vtkADIOSReader();
  ~vtkADIOSReader();

private:
  vtkADIOSReader(const vtkADIOSReader&);  // Not implemented.
  void operator=(const vtkADIOSReader&);  // Not implemented.
};

#endif
