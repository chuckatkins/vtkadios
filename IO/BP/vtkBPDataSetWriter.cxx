/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBPDataSetWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkBPDataSetWriter.h"
#include <vtkDataSet.h>
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
vtkBPDataSetWriter::vtkBPDataSetWriter()
: vtkBPDataObjectWriter()
{
}

//----------------------------------------------------------------------------
vtkBPDataSetWriter::~vtkBPDataSetWriter()
{
}

//----------------------------------------------------------------------------
void vtkBPDataSetWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkDataSet* vtkBPDataSetWriter::GetInput() const
{
  return static_cast<vtkDataSet*>(this->Superclass::GetInput());
}

//----------------------------------------------------------------------------
void vtkBPDataSetWriter::Write()
{
  this->Superclass::Write();
}

