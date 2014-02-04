/*=========================================================================

  Program:   Visualization Toolkit
  Module:    BPWriter.txx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef _BPWriter_txx_
#define _BPWriter_txx_
#include "BPWriter.h"
#include "BPUtilities.h"

//----------------------------------------------------------------------------
template<typename T>
void BPWriter::DefineVariable(const std::string &path, const T *data)
{
  this->CanDefine();

  this->ADIOSGroupSize += BPUtilities::Define<T>(this->ADIOSGroup, path, data);
}

//----------------------------------------------------------------------------
template<typename T>
void BPWriter::WriteVariable(const std::string &path, const T *data)
{
  this->IsWriting = true;

  BPUtilities::Write<T>(this->ADIOSFile, path, data);
}
#endif
