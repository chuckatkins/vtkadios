/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <map>
#include <sstream>
#include <stdexcept>
#include <limits>

#include <vtkObjectFactory.h>
#include <vtkType.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkDemandDrivenPipeline.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkMPIController.h>
#include <vtkMPI.h>

#include <vtkMultiBlockDataSet.h>
#include <vtkMultiPieceDataSet.h>
#include <vtkDataArray.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include "vtkADIOSReader.h"
#include "ADIOSVarInfo.h"

#define TEST_OBJECT_TYPE(subDir, objType) \
  if(!subDir) \
    { \
    return NULL; \
    } \
 \
  const ADIOSVarInfo *v = (*subDir)["vtkDataObjectType"]; \
  if(!(v && v->IsScalar() && v->GetValue<vtkTypeUInt8>() == objType)) \
    { \
    return NULL; \
    }

//----------------------------------------------------------------------------
typedef std::map<std::string, vtkADIOSDirTree> SubDirMap;
typedef std::map<std::string, const ADIOSVarInfo*> VarMap;
const double INVALID_STEP = std::numeric_limits<double>::min();

//----------------------------------------------------------------------------

vtkADIOSReader::vtkADIOSReader()
: FileName(""), ReadMethod(ADIOS_READ_METHOD_BP), ReadMethodArguments(""),
  Reader(NULL), NumberOfPieces(-1), Output(NULL)
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkADIOSReader::~vtkADIOSReader()
{
  delete this->Reader;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::SetController(vtkMPIController *controller)
{
  if(!controller)
    {
    vtkErrorMacro("ADIOS Reader can only be used with MPI");
    return;
    }

  this->Controller = controller;
  ADIOSReader::Initialize(*static_cast<vtkMPICommunicator *>(
    this->Controller->GetCommunicator())->GetMPIComm()->GetHandle(),
    this->ReadMethod, this->ReadMethodArguments);

  if(this->Reader)
    {
    delete this->Reader;
    }
  this->Reader = new ADIOSReader;
}

//----------------------------------------------------------------------------
int vtkADIOSReader::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkMultiBlockDataSet");
  return 1;
}

//----------------------------------------------------------------------------
int vtkADIOSReader::ProcessRequest(vtkInformation* request,
  vtkInformationVector** input, vtkInformationVector* output)
{
  if(!this->OpenAndReadMetadata())
    {
    return false;
    }

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
    this->Output = NULL;
    return this->RequestData(request, input, output);
    }

  return this->Superclass::ProcessRequest(request, input, output);
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestInformation(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkInformation* outInfo = output->GetInformationObject(0);
  outInfo->Set(vtkAlgorithm::CAN_HANDLE_PIECE_REQUEST(), 1);

  // Rank 0 reads attributes and time steps and sends to all other ranks
  if(this->Controller->GetLocalProcessId() == 0)
    {
    // 1: Retrieve the necessary attributes
    const std::vector<ADIOSAttribute*>& attrs = this->Reader->GetAttributes();
    typedef std::vector<ADIOSAttribute*>::const_iterator AttrIt;
    for(AttrIt a = attrs.begin(); a != attrs.end(); ++a)
      {
      if((*a)->GetName() == "/NumberOfPieces")
        {
        this->NumberOfPieces = (*a)->GetValue<int>();
        }
      }

    // 2: Make sure we have the ones we need
    if(this->NumberOfPieces != -1)
      {
      vtkWarningMacro(<< "NumberOfPieces attribute not present.  Assuming 1");
      this->NumberOfPieces = 1;
      }

    // 3: Retrieve the time steps
    const ADIOSVarInfo *varTimeSteps = this->Tree.Scalars["/TimeSteps"];
    const double *ptrTimeSteps = varTimeSteps->GetAllValues<double>();
    this->TimeSteps.clear();
    this->TimeSteps.reserve(varTimeSteps->GetNumSteps());
    this->TimeSteps.insert(this->TimeSteps.begin(), ptrTimeSteps,
      ptrTimeSteps + varTimeSteps->GetNumSteps());
    }

  // 4: Communicate metadata to all other ranks
  int msg1[2];
  if(this->Controller->GetLocalProcessId() == 0)
    {
    msg1[0] = this->NumberOfPieces;
    msg1[1] = this->TimeSteps.size();
    }
  this->Controller->Broadcast(msg1, 2, 0);
  if(this->Controller->GetLocalProcessId() != 0)
    {
    this->NumberOfPieces = msg1[0];
    this->TimeSteps.resize(msg1[1]);
    }
  this->Controller->Broadcast(&(*this->TimeSteps.begin()),
    this->TimeSteps.size(), 0);

  // Populate the inverse lookup, i.e. time step value to time step index
  this->TimeStepsIndex.clear();
  for(size_t i = 0; i < this->TimeSteps.size(); ++i)
    {
    this->TimeStepsIndex[this->TimeSteps[i]] = i;
    }

  // Copy the necessary values to the output info
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), 
    &(*this->TimeSteps.begin()), this->TimeSteps.size());

  double *tRange = new double[2];
  tRange[0] = *this->TimeSteps.begin();
  tRange[1] = *this->TimeSteps.rbegin();
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), tRange, 2);

  return true;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestUpdateExtent(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkInformation* outInfo = output->GetInformationObject(0);

  this->RequestNumberOfPieces = outInfo->Get(
    vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());

  this->RequestPiece = outInfo->Get(
    vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());

  this->RequestStep = outInfo->Get(
    vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
  std::map<double, size_t>::const_iterator idx = this->TimeStepsIndex.find(
    this->RequestStep);
  if(idx == this->TimeStepsIndex.end())
    {
    vtkWarningMacro(<< "Requested time step does not exist");
    return false;
    }
  this->RequestStepIndex = idx->second;

  return true;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestData(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *outputVector)
{
  // Get the output pipeline information and data object.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkMultiBlockDataSet* output = vtkMultiBlockDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Set up multi-piece for paraview 
  vtkMultiPieceDataSet *outputPieces = vtkMultiPieceDataSet::New();
  output->SetNumberOfBlocks(1);
  output->SetBlock(0, outputPieces);

  // Make sure the multi-piece has the "global view"
  outputPieces->SetNumberOfPieces(
    std::max(this->NumberOfPieces, this->RequestNumberOfPieces));

  // Cut out early if there's too many request pieces
  if(this->RequestPiece >= this->NumberOfPieces)
    {
    return true;
    }

  // Determine the range of blocks to be read
  int blockStart, blockEnd;
  int blocksPerProc = this->NumberOfPieces > this->RequestNumberOfPieces ?
    this->NumberOfPieces / this->RequestNumberOfPieces : 1;
  int blocksLeftOver = this->NumberOfPieces % blocksPerProc;
  if(this->RequestPiece < blocksLeftOver)
    {
    blockStart = (blocksPerProc + 1) * this->RequestPiece;
    blockEnd = blockStart + blocksPerProc;
    }
  else
    {
    blockStart = blocksPerProc * this->RequestPiece + blocksLeftOver;
    blockEnd = blockStart + blocksPerProc-1;
    }

  // Loop through the assigned blocks
  bool readSuccess = true;
  for(int blockId = blockStart; blockId <= blockEnd; ++blockId)
    {
    vtkDataObject *block;
    try
      {
      int objType = (*this->Tree.GetDir("/"))["vtkDataObjectType"]
        ->GetValue<vtkTypeUInt8>();
      switch(objType)
        {
        case VTK_IMAGE_DATA:
          block = this->ReadObject<vtkImageData>("/"); break;
        case VTK_POLY_DATA:
          block = this->ReadObject<vtkPolyData>("/"); break;
        case VTK_UNSTRUCTURED_GRID:
          block = this->ReadObject<vtkUnstructuredGrid>("/"); break;
        default:
          vtkErrorMacro(<< "Piece " << blockId << ": Unsupported object type");
          readSuccess = false;
          continue;
        }
      }
    catch(const std::runtime_error &e)
      {
      vtkErrorMacro(<< "Piece " << blockId << ": " << e.what());
      readSuccess = true;
      continue;
      }
    outputPieces->SetPiece(blockId, block);
    }

  // After all blocks have been scheduled, wait for the reads to process
  this->WaitForReads();

  return true;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " << this->FileName << std::endl;
  os << indent << "Tree: " << std::endl;
  this->Tree.PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::OpenAndReadMetadata(void)
{
  if(this->Reader->IsOpen())
    {
    return true;
    }

  try
    {
    this->Reader->OpenFile(this->FileName);
    this->Tree.BuildDirTree(*this->Reader);
    }
  catch(const std::runtime_error&)
    {
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::WaitForReads(void)
{
  this->Reader->ReadArrays();
}

//----------------------------------------------------------------------------
template<>
vtkImageData* vtkADIOSReader::ReadObject<vtkImageData>(
  const std::string& path)
{
  vtkADIOSDirTree *subDir = this->Tree.GetDir(path);
  TEST_OBJECT_TYPE(subDir, VTK_IMAGE_DATA)

  // Avoid excessive validation and assume that if we have a vtkDataObjectField
  // then the remainder of the subdirectory will be in proper form

  vtkImageData *data = vtkImageData::New();
  this->ReadObject(subDir, data);

  return data;
}

//----------------------------------------------------------------------------
template<>
vtkPolyData* vtkADIOSReader::ReadObject<vtkPolyData>(
  const std::string& path)
{
  vtkADIOSDirTree *subDir = this->Tree.GetDir(path);
  TEST_OBJECT_TYPE(subDir, VTK_POLY_DATA)

  // Avoid excessive validation and assume that if we have a vtkDataObjectField
  // then the remainder of the subdirectory will be in proper form

  vtkPolyData *data = vtkPolyData::New();
  this->ReadObject(subDir, data);

  return data;
}

//----------------------------------------------------------------------------
template<>
vtkUnstructuredGrid* vtkADIOSReader::ReadObject<vtkUnstructuredGrid>(
  const std::string& path)
{
  vtkADIOSDirTree *subDir = this->Tree.GetDir(path);
  TEST_OBJECT_TYPE(subDir, VTK_UNSTRUCTURED_GRID)

  // Avoid excessive validation and assume that if we have a vtkDataObjectField
  // then the remainder of the subdirectory will be in proper form

  vtkUnstructuredGrid *data = vtkUnstructuredGrid::New();
  this->ReadObject(subDir, data);

  return data;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const ADIOSVarInfo* info,
  vtkDataArray* data)
{
  std::vector<size_t> dims;
  info->GetDims(dims);
  if(dims.size() < 2)
    {
    throw std::runtime_error("Not enough dims specified for data array");
    }

  data->SetNumberOfComponents(dims[0]);
  data->SetNumberOfTuples(dims[1]);

  // Only queue the read if there's data to be read
  if(dims[0] != 0 && dims[1] != 0)
    {
    this->Reader->ScheduleReadArray(info->GetId(), data->GetVoidPointer(0),
      this->RequestStepIndex);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkCellArray* data)
{
  data->SetNumberOfCells((*subDir)["NumberOfCells"]->GetValue<vtkIdType>());
  this->ReadObject((*subDir)["Ia"], data->GetData());
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkFieldData* data)
{
  for(std::map<std::string, const ADIOSVarInfo*>::const_iterator a =
    subDir->Arrays.begin(); a != subDir->Arrays.end(); ++a)
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(a->second->GetType());

    da->SetName(a->first.c_str());
    this->ReadObject(a->second, da);
    data->AddArray(da);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkDataSetAttributes* data)
{
  const ADIOSVarInfo *v;

  if(v = (*subDir)["Scalars_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Scalars_");
    this->ReadObject(v, da);
    data->SetScalars(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["Vectors_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Vectors_");
    this->ReadObject(v, da);
    data->SetVectors(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["Normals_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Normals_");
    this->ReadObject(v, da);
    data->SetNormals(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["TCoords_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("TCoords_");
    this->ReadObject(v, da);
    data->SetTCoords(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["Tensors_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Tensors_");
    this->ReadObject(v, da);
    data->SetTensors(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["GlobalIds_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("GlobalIds_");
    this->ReadObject(v, da);
    data->SetGlobalIds(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["PedigreeIds_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("PedigreeIds_");
    this->ReadObject(v, da);
    data->SetPedigreeIds(da);
    da->UnRegister(0);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkDataSet* data)
{
  const vtkADIOSDirTree *d;

  if(d = subDir->GetDir("FieldData"))
    {
    this->ReadObject(d, data->GetFieldData());
    }
  if(d = subDir->GetDir("CellData"))
    {
    this->ReadObject(d, data->GetCellData());
    }
  if(d = subDir->GetDir("PointData"))
    {
    this->ReadObject(d, data->GetPointData());
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkImageData* data)
{
  data->SetOrigin(
    (*subDir)["OriginX"]->GetValue<double>(),
    (*subDir)["OriginY"]->GetValue<double>(),
    (*subDir)["OriginZ"]->GetValue<double>());
  data->SetSpacing(
    (*subDir)["SpacingX"]->GetValue<double>(),
    (*subDir)["SpacingY"]->GetValue<double>(),
    (*subDir)["SpacingZ"]->GetValue<double>());
  data->SetExtent(
    (*subDir)["ExtentXMin"]->GetValue<int>(),
    (*subDir)["ExtentXMax"]->GetValue<int>(),
    (*subDir)["ExtentYMin"]->GetValue<int>(),
    (*subDir)["ExtentYMax"]->GetValue<int>(),
    (*subDir)["ExtentZMin"]->GetValue<int>(),
    (*subDir)["ExtentZMax"]->GetValue<int>());

  this->ReadObject(subDir->GetDir("vtkDataSet"),
    static_cast<vtkDataSet*>(data));
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkPolyData* data)
{
  const ADIOSVarInfo *v;
  if(v = (*subDir)["Points"])
    {
    vtkPoints *p = vtkPoints::New();
    this->ReadObject(v, p->GetData());
    data->SetPoints(p);
    }

  const vtkADIOSDirTree *d;
  if(d = subDir->GetDir("Verticies"))
    {
    vtkCellArray *cells = vtkCellArray::New();
    this->ReadObject(d, cells);
    data->SetVerts(cells);
    }
  if(d = subDir->GetDir("Lines"))
    {
    vtkCellArray *cells = vtkCellArray::New();
    this->ReadObject(d, cells);
    data->SetLines(cells);
    }
  if(d = subDir->GetDir("Polygons"))
    {
    vtkCellArray *cells = vtkCellArray::New();
    this->ReadObject(d, cells);
    data->SetPolys(cells);
    }
  if(d = subDir->GetDir("Strips"))
    {
    vtkCellArray *cells = vtkCellArray::New();
    this->ReadObject(d, cells);
    data->SetStrips(cells);
    }

  this->ReadObject(subDir->GetDir("vtkDataSet"),
    static_cast<vtkDataSet*>(data));
}

//----------------------------------------------------------------------------
void vtkADIOSReader::ReadObject(const vtkADIOSDirTree *subDir,
  vtkUnstructuredGrid* data)
{
  const ADIOSVarInfo *v;
  if(v = (*subDir)["Points"])
    {
    vtkPoints *p = vtkPoints::New();
    this->ReadObject(v, p->GetData());
    data->SetPoints(p);
    }

  const ADIOSVarInfo *vCta = (*subDir)["CellTypes"];
  const ADIOSVarInfo *vCla = (*subDir)["CellLocations"];
  const vtkADIOSDirTree *dCa = subDir->GetDir("Cells");
  if(vCta && vCla && dCa)
    {
    vtkUnsignedCharArray *cta = vtkUnsignedCharArray::New();
    vtkIdTypeArray *cla = vtkIdTypeArray::New();
    vtkCellArray *ca = vtkCellArray::New();
    this->ReadObject(vCta, cta);
    this->ReadObject(vCla, cla);
    this->ReadObject(dCa, ca);
    data->SetCells(cta, cla, ca);
    }

  this->ReadObject(subDir->GetDir("vtkDataSet"),
    static_cast<vtkDataSet*>(data));
}

//----------------------------------------------------------------------------
//Cleanup
#undef TEST_OBJECT_TYPE
