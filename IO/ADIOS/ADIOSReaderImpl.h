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
  ADIOSReaderImpl(void)
  : File(NULL)
  { }

  static MPI_Comm Comm;
  static ADIOS_READ_METHOD Method;

  ADIOS_FILE* File;

  std::pair<int, int> StepRange;
  std::vector<ADIOSVarInfo*> Scalars;
  std::vector<ADIOSVarInfo*> Arrays;
  std::map<std::string, int> ArrayIds;
};

static const MPI_Comm INVALID_MPI_COMM = static_cast<MPI_Comm>(NULL);
MPI_Comm ADIOSReader::ADIOSReaderImpl::Comm = INVALID_MPI_COMM;
ADIOS_READ_METHOD ADIOSReader::ADIOSReaderImpl::Method = ADIOS_READ_METHOD_BP;
#endif
