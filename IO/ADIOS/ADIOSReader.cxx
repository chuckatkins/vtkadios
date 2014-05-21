/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ADIOSReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <cstdlib> // std::free

#include <stdexcept>
#include <map>
#include <utility>

#include "ADIOSScalar.h"
#include "ADIOSReader.h"
#include "ADIOSReaderImpl.h"
#include "ADIOSUtilities.h"
#include <adios_read.h>

typedef std::map<std::string, int> IdMap;

//----------------------------------------------------------------------------
struct ADIOSAttributeImpl
{
  ADIOSAttributeImpl(int id, const char* name, int size, ADIOS_DATATYPES type,
    void* data)
  : Id(id), Name(name), Size(size), Type(type), Data(data)
  { }

  ~ADIOSAttributeImpl(void)
  {
    // Cleanup memory that was previously alocated by ADIOS with malloc
    if(this->Data)
      {
      std::free(this->Data);
      }
  }

  int Id;
  std::string Name;
  int Size;
  ADIOS_DATATYPES Type;
  void *Data;
};

ADIOSAttribute::ADIOSAttribute(ADIOSAttributeImpl *impl)
: Impl(impl)
{ }

ADIOSAttribute::~ADIOSAttribute(void)
{ delete this->Impl; }

const std::string& ADIOSAttribute::GetName(void) const
{ return this->Impl->Name; }

int ADIOSAttribute::GetId(void) const
{ return this->Impl->Id; }

ADIOS_DATATYPES ADIOSAttribute::GetType(void) const
{ return this->Impl->Type; }

template<typename TN>
TN ADIOSAttribute::GetValue(void) const
{
  if(ADIOSUtilities::TypeNativeToADIOS<TN>::T != this->Impl->Type)
    {
    throw std::invalid_argument("Wrong type");
    }
  return *reinterpret_cast<TN*>(this->Impl->Data);
}

// Instantiations for the ADIOSAttribute::GetValue implementation
#define INSTANTIATE(T) template T ADIOSAttribute::GetValue<T>(void) const;
INSTANTIATE(int8_t)
INSTANTIATE(int16_t)
INSTANTIATE(int32_t)
INSTANTIATE(int64_t)
INSTANTIATE(uint8_t)
INSTANTIATE(uint16_t)
INSTANTIATE(uint32_t)
INSTANTIATE(uint64_t)
INSTANTIATE(float)
INSTANTIATE(double)
#undef INSTANTIATE

//----------------------------------------------------------------------------
ADIOSReader::ADIOSReader(void)
: Impl(new ADIOSReaderImpl)
{
  if(ADIOSReader::ADIOSReaderImpl::Comm == INVALID_MPI_COMM)
    {
    throw std::runtime_error("ADIOS subsystem not yet initialized");
    }
}

//----------------------------------------------------------------------------
ADIOSReader::~ADIOSReader(void)
{
  int err;

  if(this->Impl->File != NULL)
    {
    adios_read_close(this->Impl->File);
    }

  int rank = 0;
  MPI_Comm_rank(ADIOSReader::ADIOSReaderImpl::Comm, &rank);
  MPI_Barrier(ADIOSReader::ADIOSReaderImpl::Comm);

  adios_read_finalize_method(ADIOS_READ_METHOD_BP);
}

//----------------------------------------------------------------------------
bool ADIOSReader::Initialize(MPI_Comm comm, ADIOS::ReadMethod method,
  const std::string &methodArgs)
{
  if(ADIOSReader::ADIOSReaderImpl::Comm != INVALID_MPI_COMM)
    {
    return ADIOSReader::ADIOSReaderImpl::Comm == comm;
    }

  ADIOSReader::ADIOSReaderImpl::Comm = comm;
  ADIOSReader::ADIOSReaderImpl::Method = static_cast<ADIOS_READ_METHOD>(method);

  int err;
  err = adios_read_init_method(ADIOSReader::ADIOSReaderImpl::Method,
    ADIOSReader::ADIOSReaderImpl::Comm, methodArgs.c_str());
  ADIOSUtilities::TestReadErrorEq<int>(0, err);
}

//----------------------------------------------------------------------------
void ADIOSReader::OpenFile(const std::string &fileName)
{
  // Make sure we only do this once
  if(this->Impl->File)
    {
    throw std::runtime_error("ADIOSReader already has an open file.");
    }

  int err;

  // Open the file
  this->Impl->File = adios_read_open_file(fileName.c_str(),
    ADIOSReader::ADIOSReaderImpl::Method, ADIOSReader::ADIOSReaderImpl::Comm);
  ADIOSUtilities::TestReadErrorNe<void*>(NULL, this->Impl->File);

  // Poplulate step information
  this->Impl->StepRange.first = this->Impl->File->current_step;
  this->Impl->StepRange.second = this->Impl->File->last_step;

  // Preload the scalar data and cache the array metadata
  for(int i = 0; i < this->Impl->File->nvars; ++i)
    {
      ADIOS_VARINFO *v = adios_inq_var_byid(this->Impl->File, i);
      ADIOSUtilities::TestReadErrorNe<void*>(NULL, v);

      err = adios_inq_var_stat(this->Impl->File, v, 1, 1);
      ADIOSUtilities::TestReadErrorEq<int>(0, err);

      err = adios_inq_var_blockinfo(this->Impl->File, v);
      ADIOSUtilities::TestReadErrorEq<int>(0, err);

      std::string name(this->Impl->File->var_namelist[i]);

      // Insert into the appropriate scalar or array map
      if(v->ndim == 0)
        {
        this->Impl->Scalars.push_back(new ADIOSVarInfo(name, v));
        /*
        if(i == 0)
          {
          ADIOSScalar s(this->Impl->File, v);
          const double *v0 = s.GetValues<double>(0);
          const double ** const v = &v0;
          int z = 0;
          }
        */
        }
      else
        {
        this->Impl->Arrays.push_back(new ADIOSVarInfo(name, v));
        this->Impl->ArrayIds.insert(std::make_pair(name, i));
        }
    }

  // Polulate the attribute information
  for(int id = 0; id < this->Impl->File->nattrs; ++id)
    {
    ADIOS_DATATYPES type;
    int size;
    void *data;
    adios_get_attr(this->Impl->File, this->Impl->File->attr_namelist[id],
      &type, &size, &data);
    this->Impl->Attributes.push_back(new ADIOSAttribute(
      new ADIOSAttributeImpl(id, this->Impl->File->attr_namelist[id], size,
      type, data)));
    }
}

//----------------------------------------------------------------------------
void ADIOSReader::GetStepRange(int &tS, int &tE) const
{
  tS = this->Impl->StepRange.first;
  tE = this->Impl->StepRange.second;
}

//----------------------------------------------------------------------------
bool ADIOSReader::IsOpen(void) const
{
  return this->Impl->File;
}

//----------------------------------------------------------------------------
const std::vector<ADIOSAttribute*>& ADIOSReader::GetAttributes(void) const
{
  return this->Impl->Attributes;
}

//----------------------------------------------------------------------------
const std::vector<ADIOSVarInfo*>& ADIOSReader::GetScalars(void) const
{
  return this->Impl->Scalars;
}

//----------------------------------------------------------------------------
const std::vector<ADIOSVarInfo*>& ADIOSReader::GetArrays(void) const
{
  return this->Impl->Arrays;
}

//----------------------------------------------------------------------------
template<typename T>
void ADIOSReader::ScheduleReadArray(const std::string &path, T *data, int step,
  int block)
{
  IdMap::iterator id = this->Impl->ArrayIds.find(path);
  if(id != this->Impl->ArrayIds.end())
    {
    throw std::runtime_error("Array " + path + " not found");
    }
  this->ScheduleReadArray<T>(id->second, data, step, block);
}

//----------------------------------------------------------------------------
template<typename T>
void ADIOSReader::ScheduleReadArray(int id, T *data, int step, int block)
{
  int err;
  ADIOS_SELECTION *sel;

  // Use the MPI rank as the block id if not specified
  if(block == -1)
    {
    MPI_Comm_rank(ADIOSReader::ADIOSReaderImpl::Comm, &block);
    }
  sel = adios_selection_writeblock(block);

  err = adios_schedule_read_byid(this->Impl->File, sel, id,
    step, 1, data);
  ADIOSUtilities::TestReadErrorEq(0, err);
}

//----------------------------------------------------------------------------
// Instantiations for the ScheduleReadArray implementation
#define INSTANTIATE(T) \
template void ADIOSReader::ScheduleReadArray<T>(const std::string&, T*, int, int); \
template void ADIOSReader::ScheduleReadArray<T>(int, T*, int, int);
INSTANTIATE(int8_t)
INSTANTIATE(int16_t)
INSTANTIATE(int32_t)
INSTANTIATE(int64_t)
INSTANTIATE(uint8_t)
INSTANTIATE(uint16_t)
INSTANTIATE(uint32_t)
INSTANTIATE(uint64_t)
INSTANTIATE(float)
INSTANTIATE(double)
INSTANTIATE(long double)
INSTANTIATE(void)
#undef INSTANTIATE

//----------------------------------------------------------------------------
void ADIOSReader::ReadArrays(void)
{
  int err;

  err = adios_perform_reads(this->Impl->File, 1);
  ADIOSUtilities::TestReadErrorEq(0, err);
}
