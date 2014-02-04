/*=========================================================================

  Program:   Visualization Toolkit
  Module:    BPWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "BPWriter.h"
#include "BPUtilities.h"
#include <vtkImageData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include <adios.h>
#define COMMUNICATOR MPI_COMM_WORLD

//----------------------------------------------------------------------------
BPWriter::BPWriter(void)
: IsWriting(false), ADIOSGroup(BPUtilities::ADIOS_INVALID_INT64),
  ADIOSFile(BPUtilities::ADIOS_INVALID_INT64), ADIOSGroupSize(0)
{
  int err;

  err = adios_init_noxml(COMMUNICATOR);
  BPUtilities::TestError1(err, adios_get_last_errmsg());

  err = adios_allocate_buffer(ADIOS_BUFFER_ALLOC_NOW, 10);
  BPUtilities::TestError1(err, adios_get_last_errmsg());

  err = adios_declare_group(&this->ADIOSGroup, "VTK", "",
    adios_flag_yes);
  BPUtilities::TestError1(err, adios_get_last_errmsg());

  err = adios_select_method(this->ADIOSGroup, "POSIX", "", "");
  BPUtilities::TestError1(err, adios_get_last_errmsg());
}

//----------------------------------------------------------------------------
BPWriter::~BPWriter(void)
{
  int err;

  if(this->ADIOSFile != BPUtilities::ADIOS_INVALID_INT64)
    {
    adios_close(this->ADIOSFile);
    err = adios_finalize(0);
    if(err != 0)
      {
      //vtkDebugMacro(<<adios_get_last_errmsg());
      }
    }

  int rank = 0;
#ifndef _NOMPI
  MPI_Comm_rank(COMMUNICATOR, &rank);
  MPI_Barrier(COMMUNICATOR);
#endif
  adios_finalize(rank);
}

//----------------------------------------------------------------------------
void BPWriter::InitializeFile(const std::string &fileName)
{
  int err;

  err = adios_open(&this->ADIOSFile, "VTK", fileName.c_str(), "w",
    COMMUNICATOR);
  BPUtilities::TestError1(err, adios_get_last_errmsg());

  uint64_t totalSize;
  err = adios_group_size(this->ADIOSFile, this->ADIOSGroupSize,  &totalSize);
  BPUtilities::TestError1(err, adios_get_last_errmsg());
}

//----------------------------------------------------------------------------
void BPWriter::CanDefine(void)
{
  if(this->IsWriting)
    {
    throw std::runtime_error("Unable to declare variables after writing"
      " has started");
    }
}

