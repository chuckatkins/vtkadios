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

class ADIOSWriter
{
public:
  ADIOSWriter(void);
  ~ADIOSWriter(void);

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
  void DefineArray(const std::string& path, const std::vector<size_t>& dims);

  // Description
  // Define arrays for later writing
  void DefineArray(const std::string& path, const std::vector<size_t>& dims,
    int vtkType);

  // Description:
  // Open the ADIOS file and cache the variable names and scalar data
  void InitializeFile(const std::string &fileName);

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
