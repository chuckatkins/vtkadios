/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME ADIOSReader - The utility class performing ADIOS read operations

#ifndef _ADIOSReader_h
#define _ADIOSReader_h

#include <string>
#include <vector>
#include "ADIOSVarInfo.h"

class ADIOSReader
{
public:
  ADIOSReader(void);
  ~ADIOSReader(void);

  // Description:
  // Open the ADIOS file and cache the variable names and scalar data
  void InitializeFile(const std::string &fileName);

  // Description
  // Retrieve a list of scalars and thier associated metadata
  const std::vector<ADIOSVarInfo*>& GetScalars(void) const;

  // Description
  // Retrieve a list of arrays and thier associated metadata
  const std::vector<ADIOSVarInfo*>& GetArrays(void) const;

  // Description:
  // Schedule array data to be read. Data will be read with ReadArrays
  template<typename T>
  void ScheduleReadArray(const std::string &path, T *data);

  // Description:
  // Schedule array data to be read. Data will be read with ReadArrays
  template<typename T>
  void ScheduleReadArray(int id, T *data);

  // Description:
  // Perform all scheduled array read operations
  void ReadArrays(void);

protected:
  struct ADIOSReaderImpl;

  ADIOSReaderImpl *Impl;
};

#endif
