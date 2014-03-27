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
#include <map>
#include <sstream>
#include <stdexcept>

#include "vtkADIOSReader.h"
#include "ADIOSVarInfo.h"
#include <vtkObjectFactory.h>
#include <vtkDataArray.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkADIOSReader);

//----------------------------------------------------------------------------
vtkADIOSReader::vtkADIOSReader()
{
}

//----------------------------------------------------------------------------
vtkADIOSReader::~vtkADIOSReader()
{
}

//----------------------------------------------------------------------------
void vtkADIOSReader::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " << this->FileName << std::endl;
  os << indent << "Tree: " << std::endl;
  this->Tree.PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
void vtkADIOSReader::Read(void)
{
  this->Reader.InitializeFile(this->FileName);
  this->Tree.BuildDirTree(this->Reader);
}

//----------------------------------------------------------------------------
typedef std::map<std::string, vtkADIOSDirTree> SubDirMap;
typedef std::map<std::string, const ADIOSVarInfo*> VarMap;

//----------------------------------------------------------------------------
template<>
vtkDataArray* vtkADIOSReader::CreateObject<vtkDataArray>(
  const std::string& path)
{
  std::vector<std::string> pathSep;
  vtkADIOSDirTree::Tokenize(path, pathSep);
  const std::string &varName = pathSep[pathSep.size()-1];

  vtkADIOSDirTree *subDir = this->Tree.GetDir(pathSep, 1);
  if(subDir == NULL)
    {
    return NULL;
    }

  // Just a plain 'ol array
  VarMap::iterator itArray;
  if((itArray = subDir->Arrays.find(varName)) != subDir->Arrays.end())
    {
    const ADIOSVarInfo* info = itArray->second;
    vtkDataArray* data = vtkDataArray::CreateDataArray(info->GetType());
    data->SetName(varName.c_str());
    InitializeObject(info, data);
    return data;
    }

  // TODO: Add support for lookup tables

  return NULL;
}

//----------------------------------------------------------------------------
template<>
vtkImageData* vtkADIOSReader::CreateObject<vtkImageData>(
  const std::string& path)
{
  vtkADIOSDirTree *subDir = this->Tree.GetDir(path);
  if(!subDir)
    {
    return NULL;
    }

  return NULL;
}

//----------------------------------------------------------------------------
void vtkADIOSReader::InitializeObject(const ADIOSVarInfo* info,
  vtkDataArray* data)
{
  if(info->GetType() != data->GetDataType())
    {
    throw std::runtime_error("Mismatched array data types");
    }

  std::vector<size_t> dims;
  info->GetDims(dims);
  if(dims.size() < 2)
    {
    throw std::runtime_error("Not enough dims specified for data array");
    }

  data->SetNumberOfComponents(dims[0]);
  data->SetNumberOfTuples(dims[1]);
}
