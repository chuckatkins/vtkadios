/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPDataObjectWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkBPDataObjectWriter.h"
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
vtkBPDataObjectWriter::vtkBPDataObjectWriter()
: vtkBPWriter()
{
}

//----------------------------------------------------------------------------
vtkBPDataObjectWriter::~vtkBPDataObjectWriter()
{
}

//----------------------------------------------------------------------------
void vtkBPDataObjectWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkDataObject* vtkBPDataObjectWriter::GetInput() const
{
  return static_cast<vtkDataObject*>(this->Superclass::GetInput());
}

//----------------------------------------------------------------------------
void vtkBPDataObjectWriter::Write()
{
  this->Superclass::Write();
}

