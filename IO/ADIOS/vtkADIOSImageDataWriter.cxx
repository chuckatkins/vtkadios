/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSImageDataWriter->cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkADIOSImageDataWriter.h"
#include <vtkImageData.h>
#include <vtkObjectFactory.h>


vtkStandardNewMacro(vtkADIOSImageDataWriter);

//----------------------------------------------------------------------------
vtkADIOSImageDataWriter::vtkADIOSImageDataWriter()
: vtkADIOSWriter()
{
}

//----------------------------------------------------------------------------
vtkADIOSImageDataWriter::~vtkADIOSImageDataWriter()
{
}

//----------------------------------------------------------------------------
void vtkADIOSImageDataWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkImageData* vtkADIOSImageDataWriter::GetInput() const
{
  return static_cast<vtkImageData*>(this->Superclass::GetInput());
}

//----------------------------------------------------------------------------
void vtkADIOSImageDataWriter::SetInput(vtkImageData* input)
{
  this->Input = input;
}

//----------------------------------------------------------------------------
void vtkADIOSImageDataWriter::Write()
{
  vtkImageData *input = this->GetInput();
  this->Writer.DefineVariable<vtkImageData>("vtkImageData", input);
  this->Writer.InitializeFile(this->GetFileName());
  this->Writer.WriteVariable<vtkImageData>("vtkImageData", input);
}
