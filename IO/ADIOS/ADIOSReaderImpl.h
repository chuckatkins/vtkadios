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
#include <adios_read.h>
#define COMMUNICATOR MPI_COMM_WORLD

struct ADIOSReader::ADIOSReaderImpl
{
  ADIOSReaderImpl()
  : File(NULL)
  { }

  ADIOS_FILE* File;

  std::vector<ADIOSVarInfo*> Scalars;
  std::vector<ADIOSVarInfo*> Arrays;
  std::map<std::string, int> ArrayIds;
};

#endif
