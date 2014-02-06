/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSWriterVTKWrite.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "ADIOSWriter.h"
#include "ADIOSWriter.txx"
#include "ADIOSUtilities.h"
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkLookupTable.h>
#include <vtkFieldData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>

// A common pattern when the only thing being written is an array retrieved
// by a member function
#define ADIOSWriteMemberArray(path, data, getFun) \
  if((data)) \
    { \
    vtkAbstractArray *tmp = (data)->getFun(); \
    if(tmp) \
      { \
      this->WriteVariable<vtkAbstractArray>(path, tmp); \
      } \
    }

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkAbstractArray>(const std::string &path,
  const vtkAbstractArray *data)
{
  vtkAbstractArray *dataTmp = const_cast<vtkAbstractArray*>(data);

  this->IsWriting = true;
  if(!data)
    {
    return;
    }

  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/NumberOfComponents",
    dataTmp->GetNumberOfComponents());
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/NumberOfTuples",
    dataTmp->GetNumberOfTuples());
  ADIOSUtilities::Write<void>(this->ADIOSFile, path+"/Values",
    dataTmp->GetVoidPointer(0));
}

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkDataArray>(const std::string &path,
  const vtkDataArray *data)
{
  vtkDataArray *dataTmp = const_cast<vtkDataArray*>(data);

  this->IsWriting = true;
  if(!data)
    {
    return;
    }

  this->WriteVariable<vtkAbstractArray>(path+"/vtkAbstractArray", data);

  vtkLookupTable *lut = dataTmp->GetLookupTable();
  if(lut)
    {
    this->WriteVariable<vtkAbstractArray>(path+"/LookupTable", lut->GetTable());
    }
}

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkCellArray>(const std::string &path,
  const vtkCellArray *data)
{
  vtkCellArray *dataTmp = const_cast<vtkCellArray*>(data);

  this->IsWriting = true;
  if(!data)
    {
    return;
    }

  this->WriteVariable<vtkAbstractArray>(path+"/Ia", dataTmp->GetData());
}

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkFieldData>(const std::string &path,
  const vtkFieldData *data)
{
  vtkFieldData *dataTmp = const_cast<vtkFieldData*>(data);

  this->IsWriting = true;
  if(!data)
    {
    return;
    }
  
  for(size_t i = 0; i < dataTmp->GetNumberOfArrays(); ++i)
    {
    vtkDataArray *array = dataTmp->GetArray(i);
    this->WriteVariable<vtkDataArray>(path+"/"+array->GetName(), array);
    }
}

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkDataSet>(const std::string &path,
  const vtkDataSet *data)
{
  vtkDataSet *dataTmp = const_cast<vtkDataSet*>(data);

  this->IsWriting = true;
  if(!data)
    {
    return;
    }

  this->WriteVariable<vtkFieldData>(path+"/FieldData",
    dataTmp->GetFieldData());
  this->WriteVariable<vtkFieldData>(path+"/CellData",
    dataTmp->GetCellData());
  this->WriteVariable<vtkFieldData>(path+"/PointData",
    dataTmp->GetPointData());
}

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkImageData>(const std::string &path,
  const vtkImageData *data)
{
  vtkImageData *dataTmp = const_cast<vtkImageData*>(data);

  this->IsWriting = true;
  this->WriteVariable<vtkDataSet>(path+"/vtkDataSet", data);
  if(!data)
    {
    return;
    }

  double *origin = dataTmp->GetOrigin();
  ADIOSUtilities::Write<double>(this->ADIOSFile, path+"/OriginX", origin[0]);
  ADIOSUtilities::Write<double>(this->ADIOSFile, path+"/OriginY", origin[1]);
  ADIOSUtilities::Write<double>(this->ADIOSFile, path+"/OriginZ", origin[2]);

  double *spacing = dataTmp->GetSpacing();
  ADIOSUtilities::Write<double>(this->ADIOSFile, path+"/SpacingX", spacing[0]);
  ADIOSUtilities::Write<double>(this->ADIOSFile, path+"/SpacingY", spacing[1]);
  ADIOSUtilities::Write<double>(this->ADIOSFile, path+"/SpacingZ", spacing[2]);

  int *extent = dataTmp->GetExtent();
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/ExtentXMin", extent[0]);
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/ExtentXMax", extent[1]);
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/ExtentYMin", extent[2]);
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/ExtentYMax", extent[3]);
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/ExtentZMin", extent[4]);
  ADIOSUtilities::Write<int>(this->ADIOSFile, path+"/ExtentZMax", extent[5]);
}

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteVariable<vtkPolyData>(const std::string &path,
  const vtkPolyData *data)
{
  vtkPolyData *dataTmp = const_cast<vtkPolyData*>(data);

  this->IsWriting = true;
  this->WriteVariable<vtkDataSet>(path+"/vtkDataSet", data);
  if(!data)
    {
    return;
    }

  ADIOSWriteMemberArray(path+"/Points", dataTmp->GetPoints(), GetData);
  ADIOSWriteMemberArray(path+"/Vertices", dataTmp->GetVerts(), GetData);
  ADIOSWriteMemberArray(path+"/Lines", dataTmp->GetLines(), GetData);
  ADIOSWriteMemberArray(path+"/Polygons", dataTmp->GetPolys(), GetData);
  ADIOSWriteMemberArray(path+"/Strips", dataTmp->GetStrips(), GetData);
}
