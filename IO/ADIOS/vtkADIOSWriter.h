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
#include <vector>

#include "vtkIOADIOSModule.h" // For export macro
#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkDataObject.h>
#include <vtkMPIController.h>

class ADIOSWriter;

class vtkAbstractArray;
class vtkDataArray;
class vtkCellArray;
class vtkFieldData;
class vtkDataSet;
class vtkImageData;
class vtkPolyData;
class vtkUnstructuredGrid;

class VTKIOADIOS_EXPORT vtkADIOSWriter : public vtkAlgorithm
{
public:
  static vtkADIOSWriter* New(void);
  vtkTypeMacro(vtkADIOSWriter,vtkAlgorithm);
  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Get/Set the output filename
  vtkSetMacro(FileName, const std::string&);
  vtkGetMacro(FileName, const std::string&);
  //ETX

  //BTX
  // Description:
  // Get/Set the ADIOS transport method.  Current valid values are:
  // "POSIX" (default), "MPI", "MPI_LUSTRE", "MPI_AGGREGATE", "VAR_MERGE",
  // "Dataspaces", "DIMES", "PHDF5", and "NetCDF4". This is all dependent on
  // the underlying ADIOS library and the support it was built with. If
  // called, it must be called BEFORE SetController.
  vtkSetMacro(TransportMethod, const std::string&);
  vtkGetMacro(TransportMethod, const std::string&);
  //ETX

  //BTX
  // Description:
  // Get/Set arguments to the ADIOS transport method (default is "").  If
  // called, it must be called BEFORE SetController
  vtkSetMacro(TransportArguments, const std::string&);
  vtkGetMacro(TransportArguments, const std::string&);
  //ETX

  //BTX
  // Description:
  // Get/Set the data transformation.  Currently valid values are:
  // "" (default), "zlib", and "bzlib2".  Check the configuration of your
  // ADIOS library to determine the supported transform.  If called, it
  // must be called BEFORE the first step. The transform will be applied to
  // all arrays
  vtkSetMacro(Transform, const std::string&);
  vtkGetMacro(Transform, const std::string&);
  //ETX

  //BTX
  // Description:
  // Set the MPI controller.
  void SetController(vtkMPIController*);
  //ETX

  // Description:
  // Manually set the input data when not running  a pipeline
  void SetInput(vtkDataObject* input) { this->Input = input; }

  // Description:

  // Description:
  // The main interface which triggers the writer to start
  virtual int ProcessRequest(vtkInformation*, vtkInformationVector**,
    vtkInformationVector*);

  // Description:
  // Declare data if necessary and write the current step to the output stream
  virtual bool Write(void);

protected:

  // Description:
  // Define a VTK data type
  void Define(const std::string& path, const vtkAbstractArray* value);
  void Define(const std::string& path, const vtkDataArray* value);
  void Define(const std::string& path, const vtkCellArray* value);
  void Define(const std::string& path, const vtkFieldData* value);
  void Define(const std::string& path, const vtkDataSet* value);
  void Define(const std::string& path, const vtkImageData* value);
  void Define(const std::string& path, const vtkPolyData* value);
  void Define(const std::string& path, const vtkUnstructuredGrid* value);

  // Description:
  // Open a file and prepare for writing already defined variables.
  // NOTE: The data is declared only once but the file must be opened and
  // closed for every timestep.  Data is not flushed, however, until the final
  // destructor.
  void OpenFile(void);
  void CloseFile(void);

  // Description:
  // Write a previously defined VTK data type
  void Write(const std::string& path, const vtkAbstractArray* value);
  void Write(const std::string& path, const vtkDataArray* value);
  void Write(const std::string& path, const vtkCellArray* value);
  void Write(const std::string& path, const vtkFieldData* value);
  void Write(const std::string& path, const vtkDataSet* value);
  void Write(const std::string& path, const vtkImageData* value);
  void Write(const std::string& path, const vtkPolyData* value);
  void Write(const std::string& path, const vtkUnstructuredGrid* value);

  std::string FileName;
  std::string TransportMethod;
  std::string TransportArguments;
  std::string Transform;
  ADIOSWriter *Writer;
  bool FirstStep;
  int Rank;
  vtkSmartPointer<vtkMPIController> Controller;

  vtkADIOSWriter();
  ~vtkADIOSWriter();

protected:
  // Used to implement vtkAlgorithm

  int FillInputPortInformation(int port, vtkInformation* info);

  bool RequestInformation(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  bool RequestUpdateExtent(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);
  bool RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output);

  int NumberOfPieces;
  int RequestPiece;
  int NumberOfGhostLevels;
  bool WriteAllTimeSteps;
  std::vector<double> TimeSteps;
  std::vector<double>::iterator CurrentTimeStep;
  int RequestExtent[6];
  int WholeExtent[6];
  vtkSmartPointer<vtkDataObject> Input;

private:
  template<typename T>
  bool DefineAndWrite(void);

  vtkADIOSWriter(const vtkADIOSWriter&);  // Not implemented.
  void operator=(const vtkADIOSWriter&);  // Not implemented.
};

#endif
