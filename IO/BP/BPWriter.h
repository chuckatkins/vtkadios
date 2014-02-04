/*=========================================================================

  Program:   Visualization Toolkit
  Module:    BPWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME BPWriter - The utility class performing all ADIOS operations

#ifndef _BPWriter_h
#define _BPWriter_h

#include <stdint.h>
#include <cstdarg>
#include <string>
#include <stdexcept>
#include <utility>
#include <map>

class BPWriter
{
public:
  BPWriter(void);
  virtual ~BPWriter(void);

  // Description:
  // Declare the underlying variables within the associated Object
  template<typename T>
  void DefineVariable(const std::string &path, const T *data);

  // Description:
  // Set up the required ADIOS handles
  void InitializeFile(const std::string &fileName);

  // Description:
  // Write the data associated with previously declared variables aned arrays
  template<typename T>
  void WriteVariable(const std::string &path, const T *data);

protected:
  bool IsWriting;
  int64_t ADIOSFile;
  int64_t ADIOSGroup;
  int64_t ADIOSGroupSize;

  // Description:
  // Check whether or not it is safe to define a variable and throw an
  // exception if not
  void CanDefine(void);
};

#endif
