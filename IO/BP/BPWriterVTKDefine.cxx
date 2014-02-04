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

#include <sstream>

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkDataArray>(const std::string &path,
  const vtkDataArray *data)
{
  this->CanDefine();
  vtkDataArray *dataTmp = const_cast<vtkDataArray*>(data);

  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/NumberOfComponents");
  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/NumberOfTuples");

  this->ADIOSGroupSize += dataTmp->GetDataSize() * dataTmp->GetNumberOfComponents() * dataTmp->GetNumberOfTuples();
  BPUtilities::Define(this->ADIOSGroup,
    path+"/Values", (path+"/NumberOfComponents")+","+(path+"/NumberOfTuples"),
    vtkBPUtilities::VTKToADIOSType(dataTmp->GetDataType()));

  vtkLookupTable *lut = dataTmp->GetLookupTable();
  if(lut)
    {
    this->DefineVariable<vtkDataArray>(path+"/LookupTable", lut->GetTable());
    }
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkDataSetAttributes>(const std::string &path,
  const vtkDataSetAttributes *data)
{
  this->CanDefine();
  vtkDataSetAttributes *dataTmp = const_cast<vtkDataSetAttributes*>(data);

  int numArrays = dataTmp->GetNumberOfArrays();
  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/NumArrays");
  for(int i = 0; i < numArrays; ++i)
    {
    std::stringstream ss;
    ss << path << "/Array" << i;
    this->DefineVariable<vtkDataArray>(ss.str(), dataTmp->GetArray(i));
    }
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkCellData>(const std::string &path,
  const vtkCellData *data)
{
  this->CanDefine();

  this->DefineVariable<vtkDataSetAttributes>(path+"/vtkDataSetAttributes",
    data);
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkPointData>(const std::string &path,
  const vtkPointData *data)
{
  this->CanDefine();

  this->DefineVariable<vtkDataSetAttributes>(path+"/vtkDataSetAttributes",
    data);
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkInformation>(const std::string &path,
  const vtkInformation *data)
{
  this->CanDefine();
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkFieldData>(const std::string &path,
  const vtkFieldData *data)
{
  this->CanDefine();
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkDataSet>(const std::string &path,
  const vtkDataSet *data)
{
  this->CanDefine();
  vtkDataSet *dataTmp = const_cast<vtkDataSet*>(data);

  this->ADIOSGroupSize += BPUtilities::Define<double>(this->ADIOSGroup,
    path+"/Bounds", 6);
  this->DefineVariable<vtkCellData>(path+"/CellData",
    dataTmp->GetCellData());
  this->DefineVariable<vtkPointData>(path+"/PointData",
    dataTmp->GetPointData());
}

//----------------------------------------------------------------------------
template<>
void BPWriter::DefineVariable<vtkImageData>(const std::string &path,
  const vtkImageData *data)
{
  this->CanDefine();
  this->DefineVariable<vtkDataSet>(path+"/vtkDataSet", data);

  this->ADIOSGroupSize += BPUtilities::Define<double>(this->ADIOSGroup,
    path+"/Origin", 3);
  this->ADIOSGroupSize += BPUtilities::Define<double>(this->ADIOSGroup,
    path+"/Spacing", 3);
  this->ADIOSGroupSize += BPUtilities::Define<int>(this->ADIOSGroup,
    path+"/Extent", 6);
}
