/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSWriter.txx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef _ADIOSWriter_txx_
#define _ADIOSWriter_txx_
#include "ADIOSWriter.h"
#include "ADIOSUtilities.h"

//----------------------------------------------------------------------------
template<typename T>
void ADIOSWriter::DefineVariable(const std::string &path, const T *data)
{
  this->CanDefine();

  this->ADIOSGroupSize += ADIOSUtilities::Define<T>(this->ADIOSGroup, path, data);
}

//----------------------------------------------------------------------------
template<typename T>
void ADIOSWriter::WriteVariable(const std::string &path, const T *data)
{
  this->IsWriting = true;

  ADIOSUtilities::Write<T>(this->ADIOSFile, path, data);
}
#endif
