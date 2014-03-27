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
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkImageData.h>

//----------------------------------------------------------------------------
typedef std::map<std::string, vtkADIOSDirTree> SubDirMap;
typedef std::map<std::string, const ADIOSVarInfo*> VarMap;

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkADIOSReader);

//----------------------------------------------------------------------------
vtkADIOSReader::vtkADIOSReader()
{
}

//----------------------------------------------------------------------------
vtkADIOSReader::~vtkADIOSReader()
{
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
void vtkADIOSReader::Read(void)
{
  this->Reader.InitializeFile(this->FileName);
  this->Tree.BuildDirTree(this->Reader);
}

//----------------------------------------------------------------------------
template<>
vtkImageData* vtkADIOSReader::CreateObject<vtkImageData>(
  const std::string& path)
{
  vtkADIOSDirTree *subDir = this->Tree.GetDir(path);
  if(!subDir)
    {
    return NULL;
    }

  const ADIOSVarInfo *v = (*subDir)["vtkDataObjectType"];
  if(!(v && v->IsScalar() && v->GetValue<vtkTypeUInt8>() == VTK_IMAGE_DATA))
    {
    return NULL;
    }

  // Avoid excessive validation and assume that if we have a vtkDataObjectField
  // then the remainder of the subdirectory will be in proper form

  vtkImageData *data = vtkImageData::New();
  InitializeObject(subDir, data);

  return data;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const ADIOSVarInfo* info,
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
}

//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const vtkADIOSDirTree *subDir,
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

  InitializeObject(subDir->GetDir("vtkDataSet"),
    static_cast<vtkDataSet*>(data));
}
//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const vtkADIOSDirTree *subDir,
  vtkDataSet* data)
{
  const vtkADIOSDirTree *d;

  if(d = subDir->GetDir("FieldData"))
    {
    InitializeObject(d, data->GetFieldData());
    }
  if(d = subDir->GetDir("CellData"))
    {
    InitializeObject(d, data->GetCellData());
    }
  if(d = subDir->GetDir("PointData"))
    {
    InitializeObject(d, data->GetPointData());
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const vtkADIOSDirTree *subDir,
  vtkDataSetAttributes* data)
{
  const ADIOSVarInfo *v;

  if(v = (*subDir)["Scalars_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Scalars_");
    InitializeObject(v, da);
    data->SetScalars(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["Vectors_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Vectors_");
    InitializeObject(v, da);
    data->SetVectors(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["Normals_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Normals_");
    InitializeObject(v, da);
    data->SetNormals(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["TCoords_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("TCoords_");
    InitializeObject(v, da);
    data->SetTCoords(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["Tensors_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("Tensors_");
    InitializeObject(v, da);
    data->SetTensors(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["GlobalIds_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("GlobalIds_");
    InitializeObject(v, da);
    data->SetGlobalIds(da);
    da->UnRegister(0);
    }
  if(v = (*subDir)["PedigreeIds_"])
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(v->GetType());
    da->SetName("PedigreeIds_");
    InitializeObject(v, da);
    data->SetPedigreeIds(da);
    da->UnRegister(0);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const vtkADIOSDirTree *subDir,
  vtkFieldData* data)
{
  for(std::map<std::string, const ADIOSVarInfo*>::const_iterator a =
    subDir->Arrays.begin(); a != subDir->Arrays.end(); ++a)
    {
    vtkDataArray *da = vtkDataArray::CreateDataArray(a->second->GetType());

    da->SetName(a->first.c_str());
    InitializeObject(a->second, da);
    data->AddArray(da);
    }
}
