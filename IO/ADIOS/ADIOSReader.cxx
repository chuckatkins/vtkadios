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
#include <stdexcept>
#include <map>
#include <utility>

#include "ADIOSReader.h"
#include "ADIOSReaderImpl.h"
#include "ADIOSUtilities.h"

#include <adios_read.h>

typedef std::map<std::string, int> IdMap;

//----------------------------------------------------------------------------
ADIOSReader::ADIOSReader(MPI_Comm comm)
: Impl(new ADIOSReaderImpl(comm))
{
  int err;

  err = adios_read_init_method(ADIOS_READ_METHOD_BP, this->Impl->Comm, NULL);
  ADIOSUtilities::TestReadErrorEq<int>(0, err);
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
  MPI_Comm_rank(this->Impl->Comm, &rank);
  MPI_Barrier(this->Impl->Comm);

  adios_read_finalize_method(ADIOS_READ_METHOD_BP);
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
  this->Impl->File = adios_read_open(fileName.c_str(),
    ADIOS_READ_METHOD_BP, this->Impl->Comm, ADIOS_LOCKMODE_NONE, 0);
  ADIOSUtilities::TestReadErrorNe<void*>(NULL, this->Impl->File);

  // Poplulate step information
  this->Impl->StepRange.first = this->Impl->File->current_step;
  this->Impl->StepRange.second = this->Impl->File->last_step;

  // Preload the scalar data and cache the array metadata
  for(int i = 0; i < this->Impl->File->nvars; ++i)
    {
      ADIOS_VARINFO *v = adios_inq_var_byid(this->Impl->File, i);
      ADIOSUtilities::TestReadErrorNe<void*>(NULL, v);

      std::string name(this->Impl->File->var_namelist[i]);

      // Insert into the appropriate scalar or array map
      if(v->ndim == 0)
        {
        this->Impl->Scalars.push_back(new ADIOSVarInfo(name, v));
        }
      else
        {
        this->Impl->Arrays.push_back(new ADIOSVarInfo(name, v));
        this->Impl->ArrayIds.insert(std::make_pair(name, i));
        }
    }
}

//----------------------------------------------------------------------------
int ADIOSReader::Advance(void)
{
  int err;

  adios_release_step(this->Impl->File);
  err = adios_advance_step(this->Impl->File, 0, -1);
  if(err == err_end_of_stream)
    {
    return -1;
    }
  return this->GetCurrentStep();
}


//----------------------------------------------------------------------------
void ADIOSReader::GetStepRange(int &tS, int &tE) const
{
  tS = this->Impl->StepRange.first;
  tE = this->Impl->StepRange.second;
}

//----------------------------------------------------------------------------
int ADIOSReader::GetCurrentStep(void) const
{
  return this->Impl->File->current_step;
}

//----------------------------------------------------------------------------
bool ADIOSReader::IsOpen(void) const
{
  return this->Impl->File;
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
void ADIOSReader::ScheduleReadArray(const std::string &path, T *data)
{
  IdMap::iterator id = this->Impl->ArrayIds.find(path);
  if(id != this->Impl->ArrayIds.end())
    {
    throw std::runtime_error("Array " + path + " not found");
    }
  this->ScheduleReadArray<T>(id->second, data);
}

//----------------------------------------------------------------------------
template<typename T>
void ADIOSReader::ScheduleReadArray(int id, T *data)
{
  int err;
  err = adios_schedule_read_byid(this->Impl->File, NULL, id,
    0, 1, data);
  ADIOSUtilities::TestReadErrorEq(0, err);
}

//----------------------------------------------------------------------------
// Instantiations for the ScheduleReadArray implementation
#define INSTANTIATE(T) \
template void ADIOSReader::ScheduleReadArray<T>(const std::string&, T*); \
template void ADIOSReader::ScheduleReadArray<T>(int, T*);
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
