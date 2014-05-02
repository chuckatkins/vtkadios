/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSImageDataReader.cxx

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
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "vtkADIOSImageDataReader.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkADIOSImageDataReader);

//----------------------------------------------------------------------------
vtkADIOSImageDataReader::vtkADIOSImageDataReader()
: vtkADIOSReader()
{
}

//----------------------------------------------------------------------------
vtkADIOSImageDataReader::~vtkADIOSImageDataReader()
{
}

//----------------------------------------------------------------------------
vtkImageData* vtkADIOSImageDataReader::GetOutput(void)
{
  return vtkImageData::SafeDownCast(this->Output);
}

//----------------------------------------------------------------------------
int vtkADIOSImageDataReader::FillOutputPortInformation(int p, vtkInformation* i)
{
  // Only 1 port
  if(p != 0)
    {
    return 0;
    }

  i->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  return 1;
}

//----------------------------------------------------------------------------
bool vtkADIOSImageDataReader::RequestData(vtkInformation* request,
  vtkInformationVector** input, vtkInformationVector* outputVector)
{
  this->Output = this->ReadObject<vtkImageData>("/");

  return this->Superclass::RequestData(request, input, outputVector);
}
