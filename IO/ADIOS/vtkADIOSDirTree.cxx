/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkvtkADIOSDirTree.cxx

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

#include "vtkADIOSDirTree.h"

//----------------------------------------------------------------------------
// Description:
// Helper funciton to Split a string into delimited components
void vtkADIOSDirTree::Tokenize(const std::string& str,
  std::vector<std::string> &tok, char d)
{
  tok.clear();
  if(str.empty())
    {
    return;
    }

  size_t posPrev;
  for(posPrev = -1; str[posPrev+1] != d; ++posPrev); // Ignore leading delims
  size_t posCur;
  while((posCur = str.find(d, posPrev+1)) != std::string::npos)
    {
      if(posCur - posPrev > 1) // Only acknowledge non-empty path components
        {
        tok.push_back(str.substr(posPrev+1, posCur-posPrev-1));
        }
      posPrev = posCur;
    }

  if(posPrev != str.size()-1) // Only add teh last component if it's non-empty
    {
    tok.push_back(str.substr(posPrev+1, str.size()-posPrev-1));
    }
}

//----------------------------------------------------------------------------
void vtkADIOSDirTree::PrintSelf(std::ostream& os, vtkIndent indent) const
{
  typedef std::map<std::string, const ADIOSVarInfo*>::const_iterator VarMapIt;
  typedef std::map<std::string, vtkADIOSDirTree>::const_iterator DirMapIt;
  vtkIndent indent2 = indent.GetNextIndent();
  vtkIndent indent3 = indent2.GetNextIndent();

  if(this->Scalars.size() > 0)
    {
    for(VarMapIt i = this->Scalars.begin(); i != this->Scalars.end(); ++i)
      {
      os << indent << "Scalar: " << i->first << std::endl;
      }
    }

  if(this->Arrays.size() > 0)
    {
    for(VarMapIt i = this->Arrays.begin(); i != this->Arrays.end(); ++i)
      {
      os << indent << "Array: " << i->first << '[';
      std::vector<size_t> dims;
      i->second->GetDims(dims);
      for(size_t i = 0; i < dims.size()-1; ++i)
        {
        os << dims[i] << ',';
        }
      os << dims[dims.size()-1] << ']' << std::endl;
      }
    }

  for(DirMapIt i = this->SubDirs.begin(); i != this->SubDirs.end(); ++i)
    {
    os << indent << i->first << '/' << std::endl;
    i->second.PrintSelf(os, indent2);
    }
}

//----------------------------------------------------------------------------
void vtkADIOSDirTree::BuildDirTree(const ADIOSReader &reader)
{
  typedef std::vector<ADIOSVarInfo*>::const_iterator VarIt;
  std::vector<std::string> fullPath;

  // Pupulate scalars
  const std::vector<ADIOSVarInfo*>& scalars = reader.GetScalars();
  for(VarIt i = scalars.begin(); i != scalars.end(); ++i)
    {
    Tokenize((*i)->GetName(), fullPath);
    vtkADIOSDirTree *subDir = this->GetDir(fullPath, 1, true);
    subDir->Scalars[fullPath[fullPath.size()-1]] = *i;
    }

  // Pupulate arrays
  const std::vector<ADIOSVarInfo*>& arrays = reader.GetArrays();
  for(VarIt i = arrays.begin(); i != arrays.end(); ++i)
    {
    Tokenize((*i)->GetName(), fullPath);
    vtkADIOSDirTree *subDir = this->GetDir(fullPath, 1, true);
    subDir->Arrays[fullPath[fullPath.size()-1]] = *i;
    }
}

//----------------------------------------------------------------------------
vtkADIOSDirTree* vtkADIOSDirTree::GetDir(const std::string& path,
  size_t numDrop, bool createPath)
{
  std::vector<std::string> pathSplit;
  Tokenize(path, pathSplit);
  return this->GetDir(pathSplit, numDrop, createPath);
}

//----------------------------------------------------------------------------
vtkADIOSDirTree* vtkADIOSDirTree::GetDir(const std::vector<std::string>& path,
  size_t numDrop, bool createPath)
{
  typedef std::map<std::string, vtkADIOSDirTree> SubDirMap;
  
  std::stringstream ss;
  ss << '/';
  vtkADIOSDirTree* curTree = this;
  for(size_t i = 0; i < path.size() - numDrop; ++i)
    {
    const std::string &name = path[i];
    ss << name << '/';
    SubDirMap::iterator subDir = curTree->SubDirs.find(name);
    if(subDir == curTree->SubDirs.end())
      {
      if(!createPath)
        {
        return NULL;
        }
      subDir = curTree->SubDirs.insert(
        SubDirMap::value_type(name, vtkADIOSDirTree())).first;
      }
    curTree = &subDir->second;
    }
  return curTree;
}
