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

#include <limits>
#include <complex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <adios.h>

#include "ADIOSWriter.h"
#include "ADIOSUtilities.h"

#ifndef _NDEBUG
//#define DebugMacro(x)  std::cerr << "DEBUG: " << x << std::endl;
#define DebugMacro(x)
#endif

// Use an internal ADIOS function for now so we can use the transform info
extern "C" {
int64_t adios_common_define_var (int64_t group_id, const char * name
                                ,const char * path, enum ADIOS_DATATYPES type
                                ,const char * dimensions
                                ,const char * global_dimensions
                                ,const char * local_offsets
                            ,char *transform_type_str // NCSU ALACRITY-ADIOS
                                );
}

static const int64_t INVALID_INT64 = std::numeric_limits<int64_t>::min();

//----------------------------------------------------------------------------
struct ADIOSWriter::ADIOSWriterImpl
{
  ADIOSWriterImpl(void)
  : IsWriting(false), File(INVALID_INT64), Group(INVALID_INT64),
    GroupSize(0), TotalSize(0)
  {
  }

  void TestDefine(void)
  {
    if(this->IsWriting)
      {
      throw std::runtime_error("Unable to declare variables after writing"
      " has started");
      }
  }

  MPI_Comm Comm;
  bool IsWriting;
  int64_t File;
  int64_t Group;
  uint64_t GroupSize;
  uint64_t TotalSize;
};

//----------------------------------------------------------------------------
ADIOSWriter::ADIOSWriter(void)
: Impl(new ADIOSWriterImpl)
{
}

//----------------------------------------------------------------------------
bool ADIOSWriter::Initialize(MPI_Comm comm, const std::string &transport,
  const std::string &transportArgs)
{
  if(this->Impl->Group != INVALID_INT64)
    {
    return false;
    }
  this->Impl->Comm = comm;

  int err;

  err = adios_init_noxml(this->Impl->Comm);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  err = adios_allocate_buffer(ADIOS_BUFFER_ALLOC_NOW, 100);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  err = adios_declare_group(&this->Impl->Group, "VTK", "",
    adios_flag_yes);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  //err = adios_select_method(this->Impl->Group, "POSIX", "", "");
  err = adios_select_method(this->Impl->Group, transport.c_str(),
    transportArgs.c_str(), "");
  ADIOSUtilities::TestWriteErrorEq(0, err);
}

//----------------------------------------------------------------------------
ADIOSWriter::~ADIOSWriter(void)
{
  this->Close();

  int rank = 0;
  MPI_Comm_rank(this->Impl->Comm, &rank);
  adios_finalize(rank);

  delete this->Impl;
}

//----------------------------------------------------------------------------
template<typename TN>
void ADIOSWriter::DefineScalar(const std::string& path)
{
  DebugMacro( "Define Scalar: " << path);

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
  DebugMacro( "Define Scalar: " << path);

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
  const std::vector<size_t>& dims, const std::string &xfm)
{
  this->DefineArray(path, dims, ADIOSUtilities::TypeNativeToVTK<TN>::T, xfm);
}
#define INSTANTIATE(T) \
template void ADIOSWriter::DefineArray<T>(const std::string& path, \
  const std::vector<size_t>& dims, const std::string &xfm);
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
  const std::vector<size_t>& dims, int vtkType, const std::string &xfm)
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

  DebugMacro("Define Array: " << path << " [" << ssDims.str() << "]");
  int id;
  id = adios_common_define_var(this->Impl->Group, path.c_str(), "",
    adiosType, ssDims.str().c_str(), NULL, NULL, 
    xfm.empty() ? NULL : const_cast<char *>(xfm.c_str()));
  ADIOSUtilities::TestWriteErrorNe(-1, id);
  this->Impl->GroupSize += numBytes;
}

//----------------------------------------------------------------------------
void ADIOSWriter::Open(const std::string &fileName, bool append)
{
  int err;

  const char *mode = append ? "a" : "w";
  
  err = adios_open(&this->Impl->File, "VTK", fileName.c_str(), append?"a":"w",
    this->Impl->Comm);
  ADIOSUtilities::TestWriteErrorEq(0, err);

  err = adios_group_size(this->Impl->File, this->Impl->GroupSize,
    &this->Impl->TotalSize);
  ADIOSUtilities::TestWriteErrorEq(0, err);
}

//----------------------------------------------------------------------------
void ADIOSWriter::Close(void)
{
  if(this->Impl->File == INVALID_INT64)
    {
    return;
    }

  adios_close(this->Impl->File);
  this->Impl->File = INVALID_INT64;

  int rank = 0;
  MPI_Comm_rank(this->Impl->Comm, &rank);
  MPI_Barrier(this->Impl->Comm);
}

//----------------------------------------------------------------------------
template<typename TN>
void ADIOSWriter::WriteScalar(const std::string& path, const TN& value)
{
  DebugMacro( "Write Scalar: " << path);

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
  DebugMacro( "Write Scalar: " << path);

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
  DebugMacro( "Write Array: " << path);

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

