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

#include "vtkADIOSReader.h"
#include "ADIOSVarInfo.h"
#include <vtkObjectFactory.h>
#include <vtkType.h>
#include <vtkDataArray.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>

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

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkADIOSReader);

//----------------------------------------------------------------------------
vtkADIOSReader::vtkADIOSReader()
: vtkAlgorithm(), Output(NULL)
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkADIOSReader::~vtkADIOSReader()
{
}

//----------------------------------------------------------------------------
int vtkADIOSReader::ProcessRequest(vtkInformation* request,
  vtkInformationVector** input, vtkInformationVector* output)
{
  if(!this->OpenAndReadMetadata())
    {
    return false;
    }

  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
    {
    return this->RequestDataObject(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
    {
    return this->RequestInformation(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    return this->RequestData(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_NOT_GENERATED()))
    {
    return this->RequestDataNotGenerated(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_REGENERATE_INFORMATION()))
    {
    return this->RequestRegenerateInformation(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
    {
    return this->RequestUpdateExtent(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_UPDATE_TIME()))
    {
    return this->RequestUpdateTime(request, input, output);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_UPDATE_TIME_DEPENDENT_INFORMATION()))
    {
    return this->RequestUpdateTimeDependentInformation(request, input, output);
    }

  return this->Superclass::ProcessRequest(request, input, output);
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestDataObject(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not implemented");
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestInformation(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not Implemented");
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestData(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  if(!this->Object)
    {
    vtkErrorMacro("Object is not yet allocated");
    return false;
    }

  // If the object exists, then it's array reads have already been scheduled
  try
    {
    this->WaitForReads();
    }
 catch(const std::runtime_error &e)
    {
    vtkErrorMacro(e.what());
    return false;
    }

  // Get the output pipeline information and data object.
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());
  output->ShallowCopy(this->Output);

  return true;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestDataNotGenerated(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not Implemented");
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestRegenerateInformation(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not Implemented");
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestUpdateExtent(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not Implemented");
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestUpdateTime(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not Implemented");
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOSReader::RequestUpdateTimeDependentInformation(vtkInformation *req,
  vtkInformationVector **vtkNotUsed(input), vtkInformationVector *output)
{
  vtkErrorMacro("Not Implemented");
  return false;
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
  if(this->Reader.IsOpen())
    {
    return true;
    }

  try
    {
    this->Reader.InitializeFile(this->FileName);
    this->Tree.BuildDirTree(this->Reader);
    }
  catch(const std::runtime_error&)
    {
    return false;
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::WaitForReads(void)
{
  this->Reader.ReadArrays();
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
    this->Reader.ScheduleReadArray(info->GetId(), data->GetVoidPointer(0));
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
//Cleanup
#undef TEST_OBJECT_TYPE
