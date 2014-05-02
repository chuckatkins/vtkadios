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
#include <vector>

#include "ADIOSWriter.h"
#include "vtkADIOSWriter.h"
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
void vtkADIOSWriter::Define(const std::string& path, const vtkAbstractArray* v)
{
  vtkAbstractArray* valueTmp = const_cast<vtkAbstractArray*>(v);

  // String arrays not currently supported
  if(valueTmp->GetDataType() == VTK_STRING)
    {
    return;
    }

  std::vector<size_t> dims;
  dims.push_back(valueTmp->GetNumberOfComponents());
  dims.push_back(valueTmp->GetNumberOfTuples());
  this->Writer->DefineArray(path, dims, valueTmp->GetDataType(),
    this->Transform);
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
void vtkADIOSWriter::Define(const std::string& path, const vtkCellArray* v)
{
  vtkCellArray *valueTmp = const_cast<vtkCellArray*>(v);
  this->Writer->DefineScalar<vtkIdType>(path+"/NumberOfCells");
  this->Define(path+"/Ia", valueTmp->GetData());
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkFieldData* v)
{
  vtkFieldData* valueTmp = const_cast<vtkFieldData*>(v);
  for(size_t i = 0; i < valueTmp->GetNumberOfArrays(); ++i)
    {
    vtkDataArray *da = valueTmp->GetArray(i);
    vtkAbstractArray *aa = da ? da : valueTmp->GetAbstractArray(i);

    std::string name = aa->GetName();
    if(name.empty()) // skip unnamed arrays
      {
      continue;
      }
    this->Define(path+"/"+name, da ? da : aa);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkDataSet* v)
{
  vtkDataSet* valueTmp = const_cast<vtkDataSet*>(v);
  this->Writer->DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");
  this->Define(path+"/FieldData", valueTmp->GetFieldData());
  this->Define(path+"/CellData", valueTmp->GetCellData());
  this->Define(path+"/PointData", valueTmp->GetPointData());
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkImageData* v)
{
  this->Define(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  this->Writer->DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");
  this->Writer->DefineScalar<double>(path+"/OriginX");
  this->Writer->DefineScalar<double>(path+"/OriginY");
  this->Writer->DefineScalar<double>(path+"/OriginZ");
  this->Writer->DefineScalar<double>(path+"/SpacingX");
  this->Writer->DefineScalar<double>(path+"/SpacingY");
  this->Writer->DefineScalar<double>(path+"/SpacingZ");
  this->Writer->DefineScalar<int>(path+"/ExtentXMin");
  this->Writer->DefineScalar<int>(path+"/ExtentXMax");
  this->Writer->DefineScalar<int>(path+"/ExtentYMin");
  this->Writer->DefineScalar<int>(path+"/ExtentYMax");
  this->Writer->DefineScalar<int>(path+"/ExtentZMin");
  this->Writer->DefineScalar<int>(path+"/ExtentZMax");
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path, const vtkPolyData* v)
{
  this->Define(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  vtkPolyData *valueTmp = const_cast<vtkPolyData*>(v);
  this->Writer->DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");

  vtkPoints *p;
  if(p = valueTmp->GetPoints())
    {
    this->Define(path+"/Points", p->GetData());
    }

  this->Define(path+"/Verticies", valueTmp->GetVerts());
  this->Define(path+"/Lines", valueTmp->GetLines());
  this->Define(path+"/Polygons", valueTmp->GetPolys());
  this->Define(path+"/Strips", valueTmp->GetStrips());
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Define(const std::string& path,
  const vtkUnstructuredGrid* v)
{
  this->Define(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  vtkUnstructuredGrid *valueTmp = const_cast<vtkUnstructuredGrid*>(v);
  this->Writer->DefineScalar<vtkTypeUInt8>(path+"/vtkDataObjectType");

  vtkPoints *p;
  if(p = valueTmp->GetPoints())
    {
    this->Define(path+"/Points", p->GetData());
    }

  vtkCellArray *ca;
  vtkUnsignedCharArray *cta;
  if((ca = valueTmp->GetCells()) && (cta = valueTmp->GetCellTypesArray()))
    {
    this->Define(path+"/Cells", ca);
    this->Define(path+"/CellTypes", cta);
    }
}
