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
// .NAME vtkADIOSReader - Read ADIOS files.
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
#include <vtkMPIController.h>

#include "vtkIOADIOSModule.h" // For export macro
#include "vtkADIOSDirTree.h"

class ADIOSVarInfo;
class ADIOSReader;

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
  vtkTypeMacro(vtkADIOSReader,vtkAlgorithm);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the inut filename
  vtkSetMacro(FileName, const char *);  
  vtkGetMacro(FileName, const char *);  
  
  // Description:
  // Get/Set the ADIOS read method. Currently supported values are:
  // BP (default), BP_AGGREGATE, DataSpaces, DIMES, and FlexPath.  Check the
  // configuration of your ADIOS library to determine the supported read
  // methods. If called, it must be called BEFORE the first SetController.
  vtkSetMacro(ReadMethod, ADIOS_READ_METHOD);  
  vtkGetMacro(ReadMethod, ADIOS_READ_METHOD);  
  
  // Description:
  // Get/Set arguments to the ADIOS read method. Check the configuration of your
  // ADIOS library to determine the supported transform.  If called, it
  // must be called BEFORE the first SetController.
  vtkSetMacro(ReadMethodArguments, const char *);  
  vtkGetMacro(ReadMethodArguments, const char *);  
  
  // Description:
  // Set the MPI controller.
  void SetController(vtkMPIController*);

  // Description:
  // The main interface which triggers the reader to start
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
  void ReadObject(const vtkADIOSDirTree *dir, vtkUnstructuredGrid* data);

  const char *FileName;
  ADIOS_READ_METHOD ReadMethod;
  const char *ReadMethodArguments;
  vtkADIOSDirTree Tree;
  ADIOSReader *Reader;
  vtkSmartPointer<vtkMPIController> Controller;

  vtkADIOSReader();
  virtual ~vtkADIOSReader();

protected:
  // Used to implement vtkAlgorithm

  int FillOutputPortInformation(int, vtkInformation*);

  bool RequestInformation(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  bool RequestUpdateExtent(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  bool RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);

  int NumberOfPieces;
  std::vector<double> TimeSteps;
  std::map<double, size_t> TimeStepsIndex;

  double RequestStep;
  int RequestStepIndex;
  int RequestNumberOfPieces;
  int RequestPiece;
  vtkSmartPointer<vtkDataObject> Output;

private:
  vtkADIOSReader(const vtkADIOSReader&);  // Not implemented.
  void operator=(const vtkADIOSReader&);  // Not implemented.
};

#define DECLARE_EXPLICIT(T) \
template<> T* vtkADIOSReader::ReadObject<T>(const std::string& path);
DECLARE_EXPLICIT(vtkImageData)
DECLARE_EXPLICIT(vtkPolyData)
DECLARE_EXPLICIT(vtkUnstructuredGrid)
#undef DECLARE_EXPLICIT

#endif
