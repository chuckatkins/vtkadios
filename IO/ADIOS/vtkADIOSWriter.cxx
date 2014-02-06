/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkADIOSWriter.h"

//----------------------------------------------------------------------------
vtkADIOSWriter::vtkADIOSWriter()
{
}

//----------------------------------------------------------------------------
vtkADIOSWriter::~vtkADIOSWriter()
{
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkObject* vtkADIOSWriter::GetInput() const
{
  return this->Input;
}

//----------------------------------------------------------------------------
void vtkADIOSWriter::Write()
{
}

