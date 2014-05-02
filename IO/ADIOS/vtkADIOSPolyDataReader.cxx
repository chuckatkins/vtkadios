/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSPolyDataReader.cxx

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

#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "vtkADIOSPolyDataReader.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkADIOSPolyDataReader);

//----------------------------------------------------------------------------
vtkADIOSPolyDataReader::vtkADIOSPolyDataReader()
: vtkADIOSReader()
{
}

//----------------------------------------------------------------------------
vtkADIOSPolyDataReader::~vtkADIOSPolyDataReader()
{
}

//----------------------------------------------------------------------------
vtkPolyData* vtkADIOSPolyDataReader::GetOutput(void)
{
  return vtkPolyData::SafeDownCast(this->Output);
}

//----------------------------------------------------------------------------
int vtkADIOSPolyDataReader::FillOutputPortInformation(int p, vtkInformation* i)
{
  // Only 1 port
  if(p != 0)
    {
    return 0;
    }

  i->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}

//----------------------------------------------------------------------------
bool vtkADIOSPolyDataReader::RequestData(vtkInformation* request,
  vtkInformationVector** input, vtkInformationVector* outputVector)
{
  this->Output = this->ReadObject<vtkPolyData>("/");

  return this->Superclass::RequestData(request, input, outputVector);
}
