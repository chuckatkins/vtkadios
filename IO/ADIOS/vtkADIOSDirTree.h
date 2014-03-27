/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkADIOSDirTree.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkADIOSDirTree - A directory tree structure holding ADIOS data

#ifndef __vtkADIOSDirTree_h
#define __vtkADIOSDirTree_h

#include <string>
#include <vector>
#include <map>

#include <vtkIndent.h>

#include "vtkIOADIOSModule.h" // For export macro
#include "ADIOSVarInfo.h"
#include "ADIOSReader.h"

struct vtkADIOSDirTree
{
  std::map<std::string, const ADIOSVarInfo*> Scalars;
  std::map<std::string, const ADIOSVarInfo*> Arrays;
  std::map<std::string, vtkADIOSDirTree> SubDirs;

  void PrintSelf(std::ostream& os, vtkIndent indent) const;

  // Description:
  // Cosntruct a directory tree form pre-cached ADIOS data
  void BuildDirTree(const ADIOSReader &reader);

  // Description:
  // Retrieve a directory object
  vtkADIOSDirTree* GetDir(const std::string& path, size_t numDrop = 0,
    bool createPath = false);
  vtkADIOSDirTree* GetDir(const std::vector<std::string>& path,
    size_t numDrop = 0, bool createPath = false);

  static void Tokenize(const std::string& path, std::vector<std::string>& split,
    char delim='/');
};

#endif
