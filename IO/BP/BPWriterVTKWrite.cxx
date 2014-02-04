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
void BPWriter::WriteVariable<vtkDataArray>(const std::string &path,
  const vtkDataArray *data)
{
  this->IsWriting = true;
  vtkDataArray *dataTmp = const_cast<vtkDataArray*>(data);

  BPUtilities::Write<int>(this->ADIOSFile, path+"/NumberOfComponents",
    dataTmp->GetNumberOfComponents());
  BPUtilities::Write<int>(this->ADIOSFile, path+"/NumberOfTuples",
    dataTmp->GetNumberOfTuples());
  BPUtilities::Write<void>(this->ADIOSFile, path+"/Values",
    dataTmp->GetVoidPointer(0));

  vtkLookupTable *lut = dataTmp->GetLookupTable();
  if(lut)
    {
    this->WriteVariable<vtkDataArray>(path+"/LookupTable", lut->GetTable());
    }
}

//----------------------------------------------------------------------------
template<>
void BPWriter::WriteVariable<vtkCellData>(const std::string &path,
  const vtkCellData *data)
{
  this->IsWriting = true;
}

//----------------------------------------------------------------------------
template<>
void BPWriter::WriteVariable<vtkPointData>(const std::string &path,
  const vtkPointData *data)
{
  this->IsWriting = true;
}

//----------------------------------------------------------------------------
template<>
void BPWriter::WriteVariable<vtkDataSet>(const std::string &path,
  const vtkDataSet *data)
{
  this->IsWriting = true;
  vtkDataSet *dataTmp = const_cast<vtkDataSet*>(data);

  this->WriteVariable<vtkFieldData>(path+"/FieldData",
    dataTmp->GetFieldData());
  this->WriteVariable<vtkCellData>(path+"/CellData",
    dataTmp->GetCellData());
  this->WriteVariable<vtkPointData>(path+"/PointData",
    dataTmp->GetPointData());
}

//----------------------------------------------------------------------------
template<>
void BPWriter::WriteVariable<vtkImageData>(const std::string &path,
  const vtkImageData *data)
{
  this->IsWriting = true;
  this->WriteVariable<vtkDataSet>(path+"/vtkDataSet", data);
  vtkImageData *dataTmp = const_cast<vtkImageData*>(data);

  BPUtilities::Write<double>(this->ADIOSFile, path+"/Origin",
    dataTmp->GetOrigin());
  BPUtilities::Write<double>(this->ADIOSFile, path+"/Spacing",
    dataTmp->GetSpacing());
  BPUtilities::Write<int>(this->ADIOSFile, path+"/Extent",
    dataTmp->GetExtent());
}
