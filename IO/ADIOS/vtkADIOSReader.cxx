/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkADIOSReader.h"
#include "vtkADIOSReaderImpl.h"
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkADIOSReader);

//----------------------------------------------------------------------------
vtkADIOSReader::vtkADIOSReader()
: Impl(new vtkADIOSReader::vtkADIOSReaderImpl)
{
}

//----------------------------------------------------------------------------
vtkADIOSReader::~vtkADIOSReader()
{
  delete this->Impl;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " << this->FileName << std::endl;
  os << indent << "Tree: " << std::endl;
  this->Impl->Tree.PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
void vtkADIOSReader::Read(void)
{
  this->Reader.InitializeFile(this->FileName);
  this->Impl->BuildDirTree(this->Reader);
}

//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const std::string& path,
  vtkDataArray* data)
{
  
}
