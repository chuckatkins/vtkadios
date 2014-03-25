/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSReaderImpl.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef _vtkADIOSReaderImpl_h
#define _vtkADIOSReaderImpl_h

#include "ADIOSVarInfo.h"
#include "ADIOSReader.h"
#include <vector>
#include <map>
#include <utility>
#define COMMUNICATOR MPI_COMM_WORLD

// Description:
// A container reperesenting a logical subdirectory in the ADIOS tree
struct ADIOSDirTree
{
  std::map<std::string, const ADIOSVarInfo*> Scalars;
  std::map<std::string, const ADIOSVarInfo*> Arrays;
  std::map<std::string, ADIOSDirTree> SubDirs;

  void PrintSelf(std::ostream& os, vtkIndent indent) const
  {
    typedef std::map<std::string, const ADIOSVarInfo*>::const_iterator VarMapIt;
    typedef std::map<std::string, ADIOSDirTree>::const_iterator DirMapIt;
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
};

struct vtkADIOSReader::vtkADIOSReaderImpl
{
  ADIOSDirTree Tree;

  void BuildDirTree(const ADIOSReader &reader)
  {
    typedef std::vector<ADIOSVarInfo*>::const_iterator VarIt;
    std::vector<std::string> fullPath;

    // Pupulate scalars
    const std::vector<ADIOSVarInfo*>& scalars = reader.GetScalars();
    for(VarIt i = scalars.begin(); i != scalars.end(); ++i)
      {
      Tokenize((*i)->GetName(), '/', fullPath);
      ADIOSDirTree &subDir = this->GetSubDir(fullPath);
      subDir.Scalars[fullPath[fullPath.size()-1]] = *i;
      }

    // Pupulate arrays
    const std::vector<ADIOSVarInfo*>& arrays = reader.GetArrays();
    for(VarIt i = arrays.begin(); i != arrays.end(); ++i)
      {
      Tokenize((*i)->GetName(), '/', fullPath);
      ADIOSDirTree &subDir = this->GetSubDir(fullPath);
      subDir.Arrays[fullPath[fullPath.size()-1]] = *i;
      }
  }

  // Description:
  // Retrieve a subdirectory, creating the path along the way if needed
  ADIOSDirTree& GetSubDir(const std::vector<std::string>& fullPath)
  {
    typedef std::map<std::string, ADIOSDirTree> SubDirMap;

    ADIOSDirTree* curTree = &this->Tree;
    for(size_t i = 0; i < fullPath.size() - 1; ++i)
      {
      const std::string &name = fullPath[i];
      SubDirMap::iterator subDir = curTree->SubDirs.find(name);
      if(subDir == curTree->SubDirs.end())
        {
        subDir = curTree->SubDirs.insert(SubDirMap::value_type(name,
          ADIOSDirTree())).first;
        }
      curTree = &subDir->second;
      }
    return *curTree;
  }

  // Description
  // Split a string into components seperated by a give n delimiter
  static void Tokenize(const std::string& str, char d,
    std::vector<std::string> &tok)
  {
    tok.clear();

    size_t posPrev = -1;
    size_t posCur;
    while((posCur = str.find(d, posPrev+1)) != std::string::npos)
      {
        tok.push_back(str.substr(posPrev+1, posCur-posPrev-1));
        posPrev = posCur;
      }
    tok.push_back(str.substr(posPrev+1, str.size()-posPrev-1));
  }
};

#endif
