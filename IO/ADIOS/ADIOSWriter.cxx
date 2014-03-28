/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "ADIOSWriter.h"
#include "ADIOSUtilities.h"
#include <limits>
#include <complex>
#include <sstream>
#include <adios.h>
#define COMMUNICATOR MPI_COMM_WORLD

static const int64_t INVALID_INT64 = std::numeric_limits<int64_t>::min();

//----------------------------------------------------------------------------
struct ADIOSWriter::ADIOSWriterImpl
{
  ADIOSWriterImpl(void)
  : IsWriting(false), File(INVALID_INT64), Group(INVALID_INT64),
    GroupSize(0)
  { }

  void TestDefine(void)
  {
    if(this->IsWriting)
      {
      throw std::runtime_error("Unable to declare variables after writing"
      " has started");
      }
  }

  bool IsWriting;
  int64_t File;
  int64_t Group;
  int64_t GroupSize;
};

//----------------------------------------------------------------------------
ADIOSWriter::ADIOSWriter(void)
: Impl(new ADIOSWriterImpl)
{
  int err;

  err = adios_init_noxml(COMMUNICATOR);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  err = adios_allocate_buffer(ADIOS_BUFFER_ALLOC_NOW, 10);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  err = adios_declare_group(&this->Impl->Group, "VTK", "",
    adios_flag_yes);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  err = adios_select_method(this->Impl->Group, "POSIX", "", "");
  ADIOSUtilities::TestWriteErrorEq(0, err);
}

//----------------------------------------------------------------------------
ADIOSWriter::~ADIOSWriter(void)
{
  int err;

  if(this->Impl->File != INVALID_INT64)
    {
    adios_close(this->Impl->File);
    err = adios_finalize(0);
    if(err != 0)
      {
      // Debug error message
      }
    }

  int rank = 0;
#ifndef _NOMPI
  MPI_Comm_rank(COMMUNICATOR, &rank);
  MPI_Barrier(COMMUNICATOR);
#endif
  adios_finalize(rank);

  delete this->Impl;
}

//----------------------------------------------------------------------------
template<typename TN>
void ADIOSWriter::DefineScalar(const std::string& path)
{
  this->Impl->TestDefine();
  int id;
  id = adios_define_var(this->Impl->Group, path.c_str(), "",
    ADIOSUtilities::TypeNativeToADIOS<TN>::T, NULL, NULL, NULL);
  ADIOSUtilities::TestWriteErrorNe(-1, id);
  this->Impl->GroupSize += sizeof(TN);
}
#define INSTANTIATE(T) \
template void ADIOSWriter::DefineScalar<T>(const std::string& path);
INSTANTIATE(int8_t)
INSTANTIATE(int16_t)
INSTANTIATE(int32_t)
INSTANTIATE(int64_t)
INSTANTIATE(uint8_t)
INSTANTIATE(uint16_t)
INSTANTIATE(uint32_t)
INSTANTIATE(uint64_t)
INSTANTIATE(vtkIdType)
INSTANTIATE(float)
INSTANTIATE(double)
#undef INSTANTIATE

//----------------------------------------------------------------------------
void ADIOSWriter::DefineScalar(const std::string& path, const std::string& v)
{
  this->Impl->TestDefine();
  int id;
  id = adios_define_var(this->Impl->Group, path.c_str(), "",
    adios_string, NULL, NULL, NULL);
  ADIOSUtilities::TestWriteErrorNe(-1, id);
  this->Impl->GroupSize += v.size();
}

//----------------------------------------------------------------------------
template<typename TN>
void ADIOSWriter::DefineArray(const std::string& path,
  const std::vector<size_t>& dims)
{
  this->DefineArray(path, dims, ADIOSUtilities::TypeNativeToVTK<TN>::T);
}
#define INSTANTIATE(T) \
template void ADIOSWriter::DefineArray<T>(const std::string& path, \
  const std::vector<size_t>& dims);
INSTANTIATE(int8_t)
INSTANTIATE(int16_t)
INSTANTIATE(int32_t)
INSTANTIATE(int64_t)
INSTANTIATE(uint8_t)
INSTANTIATE(uint16_t)
INSTANTIATE(uint32_t)
INSTANTIATE(uint64_t)
INSTANTIATE(vtkIdType)
INSTANTIATE(float)
INSTANTIATE(double)
#undef INSTANTIATE

//----------------------------------------------------------------------------
void ADIOSWriter::DefineArray(const std::string& path,
  const std::vector<size_t>& dims, int vtkType)
{
  this->Impl->TestDefine();
  ADIOS_DATATYPES adiosType = ADIOSUtilities::TypeVTKToADIOS(vtkType);

  std::stringstream ssDims;
  size_t numBytes = ADIOSUtilities::TypeSize(adiosType);
  for(size_t i = 0; i < dims.size()-1; ++i)
    {
    ssDims << dims[i] << ',';
    numBytes *= dims[i];
    }
  ssDims << dims[dims.size()-1];
  numBytes *= dims[dims.size()-1];

  int id;
  id = adios_define_var(this->Impl->Group, path.c_str(), "",
    adiosType, ssDims.str().c_str(), NULL, NULL);
  ADIOSUtilities::TestWriteErrorNe(-1, id);
  this->Impl->GroupSize += numBytes;
}

//----------------------------------------------------------------------------
void ADIOSWriter::InitializeFile(const std::string &fileName)
{
  int err;

  err = adios_open(&this->Impl->File, "VTK", fileName.c_str(), "w",
    COMMUNICATOR);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  uint64_t totalSize;
  err = adios_group_size(this->Impl->File, this->Impl->GroupSize,  &totalSize);
  ADIOSUtilities::TestWriteErrorEq(0, err);
}

//----------------------------------------------------------------------------
template<typename TN>
void ADIOSWriter::WriteScalar(const std::string& path, const TN& value)
{
  this->Impl->IsWriting = true;

  int err;
  err = adios_write(this->Impl->File, path.c_str(), &const_cast<TN&>(value));
  ADIOSUtilities::TestWriteErrorEq(0, err);
}
#define INSTANTIATE(T) \
template void ADIOSWriter::WriteScalar<T>(const std::string& path, \
  const T& value);
INSTANTIATE(int8_t)
INSTANTIATE(int16_t)
INSTANTIATE(int32_t)
INSTANTIATE(int64_t)
INSTANTIATE(uint8_t)
INSTANTIATE(uint16_t)
INSTANTIATE(uint32_t)
INSTANTIATE(uint64_t)
INSTANTIATE(vtkIdType)
INSTANTIATE(float)
INSTANTIATE(double)
#undef INSTANTIATE

//----------------------------------------------------------------------------
template<>
void ADIOSWriter::WriteScalar<std::string>(const std::string& path,
  const std::string& value)
{
  this->Impl->IsWriting = true;

  int err;
  err = adios_write(this->Impl->File, path.c_str(),
    const_cast<char*>(value.c_str()));
  ADIOSUtilities::TestWriteErrorEq(0, err);
}

//----------------------------------------------------------------------------
template<typename TN>
void ADIOSWriter::WriteArray(const std::string& path, const TN* value)
{
  this->Impl->IsWriting = true;

  int err;
  err = adios_write(this->Impl->File, path.c_str(), const_cast<TN*>(value));
  ADIOSUtilities::TestWriteErrorEq(0, err);
}
#define INSTANTIATE(T) \
template void ADIOSWriter::WriteArray<T>(const std::string& path, \
  const T* value);
INSTANTIATE(int8_t)
INSTANTIATE(int16_t)
INSTANTIATE(int32_t)
INSTANTIATE(int64_t)
INSTANTIATE(uint8_t)
INSTANTIATE(uint16_t)
INSTANTIATE(uint32_t)
INSTANTIATE(uint64_t)
INSTANTIATE(vtkIdType)
INSTANTIATE(float)
INSTANTIATE(double)
INSTANTIATE(void)
#undef INSTANTIATE

