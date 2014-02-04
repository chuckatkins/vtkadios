/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkBPWriter.h"

//----------------------------------------------------------------------------
vtkBPWriter::vtkBPWriter()
{
}

//----------------------------------------------------------------------------
vtkBPWriter::~vtkBPWriter()
{
}

//----------------------------------------------------------------------------
void vtkBPWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkObject* vtkBPWriter::GetInput() const
{
  return this->Input;
}

//----------------------------------------------------------------------------
void vtkBPWriter::Write()
{
}

