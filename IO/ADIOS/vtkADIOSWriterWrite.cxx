/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSWriterWrite.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkADIOSWriter.h"
#include <vtkAbstractArray.h>
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkLookupTable.h>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
//----------------------------------------------------------------------------
void vtkADIOSWriter::Write(const std::string& path, const vtkAbstractArray* v)
{
  vtkAbstractArray* valueTmp = const_cast<vtkAbstractArray*>(v);

  this->Writer.WriteArray(path, valueTmp->GetVoidPointer(0));
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Write(const std::string& path, const vtkDataArray* v)
{
  vtkDataArray* valueTmp = const_cast<vtkDataArray*>(v);

  vtkLookupTable *lut = valueTmp->GetLookupTable();
  if(lut)
    {
    this->Write(path+"/LookupTable", static_cast<vtkAbstractArray*>(lut->GetTable()));
    this->Write(path+"/Values", static_cast<vtkAbstractArray*>(valueTmp));
    }
  else
    {
    this->Write(path, static_cast<vtkAbstractArray*>(valueTmp));
    }
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Write(const std::string& path, const vtkFieldData* v)
{
  vtkFieldData* valueTmp = const_cast<vtkFieldData*>(v);
  for(size_t i = 0; i < valueTmp->GetNumberOfArrays(); ++i)
    {
    vtkDataArray *array = valueTmp->GetArray(i);
    std::string name = array->GetName();
    if(name.empty()) // skip unnamed arrays
      {
      continue;
      }
    this->Write(path+"/"+name, array);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Write(const std::string& path, const vtkDataSet* v)
{
  vtkDataSet* valueTmp = const_cast<vtkDataSet*>(v);
  this->Writer.WriteScalar<vtkTypeUInt8>(path+"/vtkDataObjectType", VTK_DATA_SET);
  this->Write(path+"/FieldData", valueTmp->GetFieldData());
  this->Write(path+"/CellData", valueTmp->GetCellData());
  this->Write(path+"/PointData", valueTmp->GetPointData());
}
//----------------------------------------------------------------------------
void vtkADIOSWriter::Write(const std::string& path, const vtkImageData* v)
{
  this->Write(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  vtkImageData* valueTmp = const_cast<vtkImageData*>(v);
  this->Writer.WriteScalar<vtkTypeUInt8>(path+"/vtkDataObjectType", VTK_IMAGE_DATA);

  double *origin = valueTmp->GetOrigin();
  this->Writer.WriteScalar<double>(path+"/OriginX", origin[0]);
  this->Writer.WriteScalar<double>(path+"/OriginY", origin[1]);
  this->Writer.WriteScalar<double>(path+"/OriginZ", origin[2]);

  double *spacing = valueTmp->GetSpacing();
  this->Writer.WriteScalar<double>(path+"/SpacingX", spacing[0]);
  this->Writer.WriteScalar<double>(path+"/SpacingY", spacing[1]);
  this->Writer.WriteScalar<double>(path+"/SpacingZ", spacing[2]);

  int *extent = valueTmp->GetExtent();
  this->Writer.WriteScalar<int>(path+"/ExtentXMin", extent[0]);
  this->Writer.WriteScalar<int>(path+"/ExtentYMin", extent[1]);
  this->Writer.WriteScalar<int>(path+"/ExtentZMin", extent[2]);
  this->Writer.WriteScalar<int>(path+"/ExtentXMax", extent[3]);
  this->Writer.WriteScalar<int>(path+"/ExtentYMax", extent[4]);
  this->Writer.WriteScalar<int>(path+"/ExtentZMax", extent[5]);
}

void vtkADIOSWriter::Write(const std::string& path, const vtkPolyData* v)
{
  this->Write(path+"/vtkDataSet", static_cast<const vtkDataSet*>(v));

  vtkPolyData* valueTmp = const_cast<vtkPolyData*>(v);
  this->Writer.WriteScalar<vtkTypeUInt8>(path+"/vtkDataObjectType", VTK_POLY_DATA);
  if(valueTmp->GetPoints())
    {
    this->Write(path+"/Points", valueTmp->GetPoints()->GetData());
    }
  if(valueTmp->GetVerts())
    {
    this->Write(path+"/Verticies", valueTmp->GetVerts()->GetData());
    }
  if(valueTmp->GetLines())
    {
    this->Write(path+"/Lines", valueTmp->GetLines()->GetData());
    }
  if(valueTmp->GetPolys())
    {
    this->Write(path+"/Polygons", valueTmp->GetPolys()->GetData());
    }
  if(valueTmp->GetStrips())
    {
    this->Write(path+"/Strips", valueTmp->GetStrips()->GetData());
    }  
}
