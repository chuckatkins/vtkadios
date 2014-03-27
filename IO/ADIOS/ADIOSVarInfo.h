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
// .NAME ADIOSVarInfo - The utility class wrapping the ADIOS_VARINFO struct

#ifndef _ADIOSVarInfo_h
#define _ADIOSVarInfo_h

#include <string>
#include <vector>

//----------------------------------------------------------------------------
class ADIOSVarInfo
{
public:
  ADIOSVarInfo(const std::string &name = "", void* var = NULL);
  ~ADIOSVarInfo(void);

  std::string GetName(void) const;
  int GetId(void) const;
  int GetType(void) const;
  size_t GetNumSteps(void) const;
  bool IsGlobal(void) const;
  bool IsScalar(void) const;
  void GetDims(std::vector<size_t>& dims) const;

  template<typename T>
  T GetValue(void) const;

private:
  struct ADIOSVarInfoImpl;
  ADIOSVarInfoImpl *Impl;
};

#endif // _ADIOSVarInfo_h
