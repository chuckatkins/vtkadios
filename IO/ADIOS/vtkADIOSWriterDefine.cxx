/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSWriterDefine.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkADIOSWriter.h"
#include <vector>
#include <vtkAbstractArray.h>
#include <vtkLookupTable.h>
#include <vtkDataArray.h>
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkAbstractArray* v)
{
  vtkAbstractArray* valueTmp = const_cast<vtkAbstractArray*>(v);

  std::vector<size_t> dims;
  dims.push_back(valueTmp->GetNumberOfComponents());
  dims.push_back(valueTmp->GetNumberOfTuples());
  this->Writer.DefineArray(path, dims, valueTmp->GetDataType());
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkDataArray* v)
{
  vtkDataArray* valueTmp = const_cast<vtkDataArray*>(v);

  vtkLookupTable *lut = valueTmp->GetLookupTable();
  if(lut)
    {
/*
    this->Define(path+"/LookupTable", static_cast<vtkAbstractArray*>(lut->GetTable()));
    this->Define(path+"/Values", static_cast<vtkAbstractArray*>(valueTmp));
*/
    }
  else
    {
    this->Define(path, static_cast<vtkAbstractArray*>(valueTmp));
    }
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkFieldData* v)
{
  vtkFieldData* valueTmp = const_cast<vtkFieldData*>(v);
  for(size_t i = 0; i < valueTmp->GetNumberOfArrays(); ++i)
    {
    vtkDataArray *array = valueTmp->GetArray(i);
    if(!array) // Currently string arrays are not supported
      {
      continue;
      }

    std::string name = array->GetName();
    if(name.empty()) // skip unna med arrays
      {
      continue;
      }
    this->Define(path+"/"+name, array);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkDataSet* v)
{
  vtkDataSet* valueTmp = const_cast<vtkDataSet*>(v);
  this->Writer.DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");
  this->Define(path+"/FieldData", valueTmp->GetFieldData());
  this->Define(path+"/CellData", valueTmp->GetCellData());
  this->Define(path+"/PointData", valueTmp->GetPointData());
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkImageData* v)
{
  this->Define(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  this->Writer.DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");
  this->Writer.DefineScalar<double>(path+"/OriginX");
  this->Writer.DefineScalar<double>(path+"/OriginY");
  this->Writer.DefineScalar<double>(path+"/OriginZ");
  this->Writer.DefineScalar<double>(path+"/SpacingX");
  this->Writer.DefineScalar<double>(path+"/SpacingY");
  this->Writer.DefineScalar<double>(path+"/SpacingZ");
  this->Writer.DefineScalar<int>(path+"/ExtentXMin");
  this->Writer.DefineScalar<int>(path+"/ExtentXMax");
  this->Writer.DefineScalar<int>(path+"/ExtentYMin");
  this->Writer.DefineScalar<int>(path+"/ExtentYMax");
  this->Writer.DefineScalar<int>(path+"/ExtentZMin");
  this->Writer.DefineScalar<int>(path+"/ExtentZMax");
}

void vtkADIOSWriter::Define(const std::string& path, const vtkPolyData* v)
{
  this->Define(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  vtkPolyData *valueTmp = const_cast<vtkPolyData*>(v);
  this->Writer.DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");
  if(valueTmp->GetPoints())
    {
    this->Define(path+"/Points", valueTmp->GetPoints()->GetData());
    }
  if(valueTmp->GetVerts())
    {
    this->Define(path+"/Verticies", valueTmp->GetVerts()->GetData());
    }
  if(valueTmp->GetLines())
    {
    this->Define(path+"/Lines", valueTmp->GetLines()->GetData());
    }
  if(valueTmp->GetPolys())
    {
    this->Define(path+"/Polygons", valueTmp->GetPolys()->GetData());
    }
  if(valueTmp->GetStrips())
    {
    this->Define(path+"/Strips", valueTmp->GetStrips()->GetData());
    }
}
