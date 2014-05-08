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
#include <utility>
#include "ADIOSVarInfo.h"
#include <adios_mpi.h>

class ADIOSReader
{
public:
  ADIOSReader(void);
  ~ADIOSReader(void);

  // Description:
  // Initialize the underlying ADIOS subsystem
  static bool Initialize(MPI_Comm comm, const std::string &method = "BP",
    const std::string &methodArgs = "");

  // Description:
  // Open the ADIOS file and cache the variable names and scalar data
  void OpenFile(const std::string &fileName);

  // Description:
  // Retrieve the total number of seps
  void GetStepRange(int &tStart, int &tEnd) const;

  // Description
  // Retrieve a list of scalars and thier associated metadata
  const std::vector<ADIOSVarInfo*>& GetScalars(void) const;

  // Description
  // Retrieve a list of arrays and thier associated metadata
  const std::vector<ADIOSVarInfo*>& GetArrays(void) const;

  // Description:
  // Schedule array data to be read. Data will be read with ReadArrays
  template<typename T>
  void ScheduleReadArray(const std::string &path, T *data, int step);

  // Description:
  // Schedule array data to be read. Data will be read with ReadArrays
  template<typename T>
  void ScheduleReadArray(int id, T *data, int step);

  // Description:
  // Perform all scheduled array read operations
  void ReadArrays(void);

  // Description:
  // Whether or not the file / stream is already open
  bool IsOpen(void) const;

protected:
  struct ADIOSReaderImpl;

  ADIOSReaderImpl *Impl;
};

#endif
