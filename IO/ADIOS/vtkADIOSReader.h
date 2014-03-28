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
#include <vector>
#include <map>

#include <vtkObject.h>

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSDirTree.h"
#include "ADIOSVarInfo.h"
#include "ADIOSReader.h"

class vtkDataArray;
class vtkImageData;
class vtkDataSet;
class vtkDataSetAttributes;
class vtkFieldData;
class vtkPolyData;

//----------------------------------------------------------------------------

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
  // Create a VTK object with it's scalar values and allocate any arrays, and
  // schedule them for reading
  template<typename T>
  T* ReadObject(const std::string& path);

protected:

  // Description:
  // Initialize a pre-allocated object with it's appropriate scalars.  These
  // methods do not perform any validation and assume that the provides ADIOS
  // structures and vtk objects are properly formed.  Arrays will be scheduled
  // for reading afterwards
  void ReadObject(const ADIOSVarInfo* info, vtkDataArray* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkImageData* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkDataSet* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkDataSetAttributes* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkFieldData* data);

  std::string FileName;
  vtkADIOSDirTree Tree;
  ADIOSReader Reader;

  vtkADIOSReader();
  ~vtkADIOSReader();

private:
  vtkADIOSReader(const vtkADIOSReader&);  // Not implemented.
  void operator=(const vtkADIOSReader&);  // Not implemented.
};

#define DECLARE_EXPLICIT(T) \
template<> T* vtkADIOSReader::ReadObject<T>(const std::string& path);
DECLARE_EXPLICIT(vtkImageData)
#undef DECLARE_EXPLICIT

#endif
