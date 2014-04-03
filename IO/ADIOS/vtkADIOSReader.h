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

#include <vtkAlgorithm.h>
#include <vtkSetGet.h>
#include <vtkSmartPointer.h>

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSDirTree.h"
#include "ADIOSVarInfo.h"
#include "ADIOSReader.h"

class vtkDataArray;
class vtkCellArray;
class vtkFieldData;
class vtkDataSetAttributes;
class vtkDataObject;
class vtkDataSet;
class vtkImageData;
class vtkPolyData;

//----------------------------------------------------------------------------

class VTKIOADIOS_EXPORT vtkADIOSReader : public vtkAlgorithm
{
public:
  static vtkADIOSReader* New();
  vtkTypeMacro(vtkADIOSReader,vtkObject);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Get/Set the output filename
  vtkSetMacro(FileName, const std::string&);  
  vtkGetMacro(FileName, const std::string&);  
  //ETX
  
  // Description:
  // The main interface which triggers the reader to start
  // TODO: Make pure virtual and only implement in derived concrete readers
  virtual int ProcessRequest(vtkInformation*, vtkInformationVector**,
    vtkInformationVector*);

protected:

  // Description:
  // Open an ADIOS file and build the directory structure
  bool OpenAndReadMetadata(void);

  // Description:
  // Wait for all scheduled array reads to finish
  void WaitForReads(void);

  // Description:
  // Create a VTK object with it's scalar values and allocate any arrays, and
  // schedule them for reading
  template<typename T>
  T* ReadObject(const std::string& path);

  // Description:
  // Initialize a pre-allocated object with it's appropriate scalars.  These
  // methods do not perform any validation and assume that the provides ADIOS
  // structures and vtk objects are properly formed.  Arrays will be scheduled
  // for reading afterwards
  void ReadObject(const ADIOSVarInfo* info, vtkDataArray* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkCellArray* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkFieldData* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkDataSetAttributes* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkDataSet* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkImageData* data);
  void ReadObject(const vtkADIOSDirTree *dir, vtkPolyData* data);

  std::string FileName;
  vtkADIOSDirTree Tree;
  ADIOSReader Reader;

  vtkADIOSReader();
  ~vtkADIOSReader();

protected:
  // Uset to implement vtkAlgorithm

  virtual int ProcessRequest(vtkInformation *request,
   vtkInformationVector **input, vtkInformationVector *output);

  virtual bool RequestDataObject(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  virtual bool RequestInformation(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  virtual bool RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  virtual bool RequestDataNotGenerated(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  virtual bool RequestRegenerateInformation(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);

  virtual bool RequestUpdateExtent(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  virtual bool RequestUpdateTime(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  virtual bool RequestUpdateTimeDependentInformation(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);

  

  vtkSmartPointer<vtkDataObject> Output;

private:
  vtkADIOSReader(const vtkADIOSReader&);  // Not implemented.
  void operator=(const vtkADIOSReader&);  // Not implemented.
};

#define DECLARE_EXPLICIT(T) \
template<> T* vtkADIOSReader::ReadObject<T>(const std::string& path);
DECLARE_EXPLICIT(vtkImageData)
DECLARE_EXPLICIT(vtkPolyData)
#undef DECLARE_EXPLICIT

#endif
