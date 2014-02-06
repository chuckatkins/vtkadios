/*=========================================================================

  Program:   Visualization Toolkit
  Module:    BPWriterVTKWrite.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "BPWriter.h"
#include "BPWriter.txx"
#include "BPUtilities.h"
#include <vtkImageData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkLookupTable.h>

#include <sstream>

//----------------------------------------------------------------------------
template<>
void BPWriter::WriteVariable<vtkAbstractArray>(const std::string &path,
  const vtkAbstractArray *data)
{
  vtkAbstractArray *dataTmp = const_cast<vtkAbstractArray*>(data);

  this->IsWriting = true;
  if(!data)
    {
    return;
    }

  //BPUtilities::Write<std::string>(this->ADIOSGroup, path+"/Name",
  //  dataTmp->GetName());
  BPUtilities::Write<int>(this->ADIOSFile, path+"/NumberOfComponents",
    dataTmp->GetNumberOfComponents());
  BPUtilities::Write<int>(this->ADIOSFile, path+"/NumberOfTuples",
    dataTmp->GetNumberOfTuples());
  BPUtilities::Write<void>(this->ADIOSFile, path+"/Values",
    dataTmp->GetVoidPointer(0));
}

//----------------------------------------------------------------------------
template<>
void BPWriter::WriteVariable<vtkDataArray>(const std::string &path,
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
void BPWriter::WriteVariable<vtkFieldData>(const std::string &path,
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
void BPWriter::WriteVariable<vtkDataSet>(const std::string &path,
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
void BPWriter::WriteVariable<vtkImageData>(const std::string &path,
  const vtkImageData *data)
{
  vtkImageData *dataTmp = const_cast<vtkImageData*>(data);

  this->IsWriting = true;
  this->WriteVariable<vtkDataSet>(path+"/vtkDataSet", data);
  if(!data)
    {
    return;
    }

  BPUtilities::Write<double>(this->ADIOSFile, path+"/Origin",
    dataTmp->GetOrigin());
  BPUtilities::Write<double>(this->ADIOSFile, path+"/Spacing",
    dataTmp->GetSpacing());
  BPUtilities::Write<int>(this->ADIOSFile, path+"/Extent",
    dataTmp->GetExtent());
}
