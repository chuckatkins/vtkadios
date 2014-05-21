/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <cstring>
#include <limits>
#include <stdexcept>
#include <iostream>

#include "ADIOSWriter.h"

#include "vtkADIOSWriter.h"
#include <vtkObjectFactory.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkDemandDrivenPipeline.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkMPIController.h>
#include <vtkMPI.h>

#include <vtkDataObject.h>
#include <vtkAbstractArray.h>
#include <vtkLookupTable.h>
#include <vtkDataArray.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>


//----------------------------------------------------------------------------
const double INVALID_STEP = std::numeric_limits<double>::min();

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkADIOSWriter);

//----------------------------------------------------------------------------
vtkADIOSWriter::vtkADIOSWriter()
: FileName(""), TransportMethod(ADIOS::TransportMethod_POSIX),
  TransportMethodArguments(""), Transform(ADIOS::Transform_NONE),
  Writer(NULL), Controller(NULL),
  NumberOfPieces(-1), RequestPiece(-1), NumberOfGhostLevels(-1),
  WriteAllTimeSteps(true), TimeSteps(), CurrentTimeStep(TimeSteps.begin())
{
  std::memset(this->RequestExtent, 0, 6*sizeof(int));
  std::memset(this->WholeExtent, 0, 6*sizeof(int));
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(0);
}

//----------------------------------------------------------------------------
vtkADIOSWriter::~vtkADIOSWriter()
{
  delete this->Writer;
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " << this->FileName << std::endl;
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::SetController(vtkMPIController *controller)
{
  if(!controller)
    {
    vtkErrorMacro("ADIOS Writer can only be used with MPI");
    return;
    }

  this->Controller = controller;
  ADIOSWriter::Initialize(*static_cast<vtkMPICommunicator *>(
      this->Controller->GetCommunicator())->GetMPIComm()->GetHandle());

  if(this->Writer)
    {
    delete this->Writer;
    }
  this->Writer = new ADIOSWriter(this->TransportMethod,
    this->TransportMethodArguments);
  this->NumberOfPieces = this->Controller->GetNumberOfProcesses();
  this->RequestPiece = this->Controller->GetLocalProcessId();
  this->FirstStep = true;
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::OpenFile(void)
{
  this->Writer->Open(this->FileName, !this->FirstStep);
  this->FirstStep = false;
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::CloseFile(void)
{
  this->Writer->Close();
}

//----------------------------------------------------------------------------
template<typename T>
bool vtkADIOSWriter::DefineAndWrite(vtkDataObject *input)
{
  const T *data = T::SafeDownCast(input);
  if(!data)
    {
    return false;
    }

  try
    {
    // Things to do on the first step, before writing any data
    if(this->FirstStep)
      {
      // 1: Rank 0 must declare any attributes
      if(this->RequestPiece == 0)
        {
        this->Writer->DefineAttribute<int>("/NumberOfPieces",
          this->NumberOfPieces);
        }

      // 2: Before any data can be writen, it's structure must be declared
      this->Writer->DefineScalar<double>("/TimeStamp");
      this->Define("", data);
      }

    // Make sure we're within time bounds
    if(this->CurrentTimeStep == this->TimeSteps.end())
      {
      vtkErrorMacro(<< "All timesteps have been exhausted");
      return false;
      }

    this->OpenFile();
    if(this->RequestPiece == 0)
      {
      vtkWarningMacro(<< "Writing data for time " << (*this->CurrentTimeStep)*1e6);
      this->Writer->WriteScalar<double>("/TimeStamp", *this->CurrentTimeStep);
      }
    this->Write("", data);
    this->CloseFile();
    }
  catch(const std::runtime_error &err)
    {
    vtkErrorMacro(<< err.what());
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool vtkADIOSWriter::WriteInternal(void)
{
  vtkDataObject *input = this->GetInputDataObject(0, 0);
  if(!input)
    {
    return false;
    }

  switch(input->GetDataObjectType())
    {
    case VTK_IMAGE_DATA:
      return this->DefineAndWrite<vtkImageData>(input);
    case VTK_POLY_DATA:
      return this->DefineAndWrite<vtkPolyData>(input);
    case VTK_UNSTRUCTURED_GRID:
      return this->DefineAndWrite<vtkUnstructuredGrid>(input);
    default:
      vtkErrorMacro("Input vtkDataObject type not supported by ADIOS writer");
      return false;
    }
}

//----------------------------------------------------------------------------
int vtkADIOSWriter::FillInputPortInformation(int port, vtkInformation *info)
{
  // Only 1 port
  if(port != 0)
    {
    return 0;
    }

  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
  return 1;
}

//----------------------------------------------------------------------------
int vtkADIOSWriter::ProcessRequest(vtkInformation* request,
  vtkInformationVector** input, vtkInformationVector* output)
{
  // Make sure the ADIOS subsystem is initialized before processing any
  // srt of request.  If no controller has been specified then assume the
  //global MPI Controller
  if(!this->Controller)
    {
    this->SetController(vtkMPIController::SafeDownCast(
      vtkMultiProcessController::GetGlobalController()));
    }

  // Now process the request

  if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
    {
    return this->RequestInformation(request, input, output);
    }

  if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
    {
    return this->RequestUpdateExtent(request, input, output);
    }

  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    return this->RequestData(request, input, output);
    }

  return this->Superclass::ProcessRequest(request, input, output);
}

//----------------------------------------------------------------------------
bool vtkADIOSWriter::RequestInformation(vtkInformation *req,
  vtkInformationVector **input, vtkInformationVector *vtkNotUsed(output))
{
  vtkInformation *inInfo = input[0]->GetInformationObject(0);

  if (inInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()))
    {
    int len = inInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());

    double *steps = inInfo->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    this->TimeSteps.clear();
    this->TimeSteps.reserve(len);
    this->TimeSteps.insert(this->TimeSteps.begin(), steps, steps+len);
    this->CurrentTimeStep = this->TimeSteps.begin();
    }

  return true;
}

//----------------------------------------------------------------------------
bool vtkADIOSWriter::RequestUpdateExtent(vtkInformation *req,
  vtkInformationVector **input, vtkInformationVector *vtkNotUsed(output))
{
  vtkInformation* inInfo = input[0]->GetInformationObject(0);
 
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(),
    this->NumberOfPieces);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(),
    this->RequestPiece);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(),
      *this->CurrentTimeStep);
  return true;
}

//----------------------------------------------------------------------------
bool vtkADIOSWriter::RequestData(vtkInformation *req,
  vtkInformationVector **input, vtkInformationVector *vtkNotUsed(output))
{
  vtkInformation* inInfo = input[0]->GetInformationObject(0);

  // Start looping if we're at the beginning
  if(this->CurrentTimeStep == this->TimeSteps.begin() &&
      this->WriteAllTimeSteps)
    {
    req->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
    }

  if(!this->WriteInternal())
    {
    return false;
    }

  // End looping if we're at the end
  if(++this->CurrentTimeStep == this->TimeSteps.end() &&
     this->WriteAllTimeSteps)
    {
    req->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
    }

  return true;
}

