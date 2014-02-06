/*=========================================================================

  Program:   Visualization Toolkit
  Module:    BPWriterVTKDefine.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "BPWriter.h"
#include "BPUtilities.h"
#include "BPUtilitiesVTK.h"
#include <vtkImageData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkLookupTable.h>
#include <vtkFieldData.h>

#include <sstream>

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkAbstractArray>(const std::string &path,
  const vtkAbstractArray *data)
{
  vtkAbstractArray *dataTmp = const_cast<vtkAbstractArray*>(data);

  this->CanDefine();
  if(!data || dataTmp->GetNumberOfTuples() == 0)
    {
    return;
    }

  //this->ADIOSGroupSize += BPUtilities::Define<std::string>(this->ADIOSGroup,
  //  path+"/Name", dataTmp->GetName());
  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/NumberOfComponents");
  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/NumberOfTuples");

  this->ADIOSGroupSize += dataTmp->GetDataTypeSize() *
                          dataTmp->GetNumberOfComponents() *
                          dataTmp->GetNumberOfTuples();
  BPUtilities::Define(this->ADIOSGroup,
    path+"/Values", (path+"/NumberOfComponents")+","+(path+"/NumberOfTuples"),
    vtkBPUtilities::VTKToADIOSType(dataTmp->GetDataType()));
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkDataArray>(const std::string &path,
  const vtkDataArray *data)
{
  vtkDataArray *dataTmp = const_cast<vtkDataArray*>(data);

  this->CanDefine();
  if(!data || dataTmp->GetNumberOfTuples() == 0)
    {
    return;
    }

  this->DefineVariable<vtkAbstractArray>(path+"/vtkAbstractArray", data);

  vtkLookupTable *lut = dataTmp->GetLookupTable();
  if(lut)
    {
    this->DefineVariable<vtkAbstractArray>(path+"/LookupTable", lut->GetTable());
    }
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkFieldData>(const std::string &path,
  const vtkFieldData *data)
{
  vtkFieldData *dataTmp = const_cast<vtkFieldData*>(data);

  this->CanDefine();
  if(!data)
    {
    return;
    }

  for(size_t i = 0; i < dataTmp->GetNumberOfArrays(); ++i)
    {
    vtkDataArray *array = dataTmp->GetArray(i);
    this->DefineVariable<vtkDataArray>(path+"/"+array->GetName(), array);
    }
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkDataSet>(const std::string &path,
  const vtkDataSet *data)
{
  vtkDataSet *dataTmp = const_cast<vtkDataSet*>(data);

  this->CanDefine();
  if(!data)
    {
    return;
    }

  this->DefineVariable<vtkFieldData>(path+"/FieldData",
    dataTmp->GetFieldData());
  this->DefineVariable<vtkFieldData>(path+"/CellData",
    dataTmp->GetCellData());
  this->DefineVariable<vtkFieldData>(path+"/PointData",
    dataTmp->GetPointData());
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkImageData>(const std::string &path,
  const vtkImageData *data)
{
  this->CanDefine();
  this->DefineVariable<vtkDataSet>(path+"/vtkDataSet", data);
  if(!data)
    {
    return;
    }

  this->ADIOSGroupSize += BPUtilities::Define<double>(this->ADIOSGroup,
    path+"/Origin", 3);
  this->ADIOSGroupSize += BPUtilities::Define<double>(this->ADIOSGroup,
    path+"/Spacing", 3);
  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/Extent", 6);
}
