/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME ADIOSWriter - The utility class performing ADIOS Write operations

#ifndef _ADIOSWriter_h
#define _ADIOSWriter_h

#include <string>
#include <vector>
#include <adios_mpi.h>

class ADIOSWriter
{
public:
  ADIOSWriter(const std::string &transport = "POSIX",
    const std::string &transportArgs = "");

  ~ADIOSWriter(void);

  // Description:
  // Initialize the underlying ADIOS subsystem
  static bool Initialize(MPI_Comm comm);

  // Description
  // Define scalar attributes
  template<typename TN>
  void DefineAttribute(const std::string& path, const TN& value);

  // Description
  // Define scalars for later writing
  template<typename TN>
  void DefineScalar(const std::string& path);

  // Description
  // Define scalars for later writing
  void DefineScalar(const std::string& path, const std::string& v);

  // Description
  // Define arrays for later writing
  template<typename TN>
  void DefineArray(const std::string& path, const std::vector<size_t>& dims,
    const std::string &xfm = "");

  // Description
  // Define arrays for later writing
  void DefineArray(const std::string& path, const std::vector<size_t>& dims,
    int vtkType, const std::string &xfm = "");

  // Description:
  // Open the vtk group in the ADIOS file for writing one timestep
  void Open(const std::string &fileName, bool append = false);

  // Description:
  // Close the VTK group for the current time step in the ADIOS file
  void Close(void);

  // Description
  // Schedule scalars for writing
  template<typename TN>
  void WriteScalar(const std::string& path, const TN& value);

  // Description
  // Schedule arrays for writing
  template<typename TN>
  void WriteArray(const std::string& path, const TN* value);

protected:
  struct ADIOSWriterImpl;

  ADIOSWriterImpl *Impl;
};

#endif
