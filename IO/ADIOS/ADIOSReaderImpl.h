/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef _ADIOSReaderImpl_h
#define _ADIOSReaderImpl_h

#include "ADIOSReader.h"
#include "ADIOSVarInfo.h"
#include <vector>
#include <map>
#include <utility>
#include <adios_read.h>

struct ADIOSReader::ADIOSReaderImpl
{
  ADIOSReaderImpl(MPI_Comm comm)
  : File(NULL), Comm(comm)
  { }

  ADIOS_FILE* File;
  MPI_Comm Comm;

  std::pair<int, int> StepRange;
  std::vector<ADIOSVarInfo*> Scalars;
  std::vector<ADIOSVarInfo*> Arrays;
  std::map<std::string, int> ArrayIds;
};

#endif
