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
#include <vtkImageData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include <adios.h>
#define COMMUNICATOR MPI_COMM_WORLD

//----------------------------------------------------------------------------
ADIOSWriter::ADIOSWriter(void)
: IsWriting(false), ADIOSGroup(ADIOSUtilities::ADIOS_INVALID_INT64),
  ADIOSFile(ADIOSUtilities::ADIOS_INVALID_INT64), ADIOSGroupSize(0)
{
  int err;

  err = adios_init_noxml(COMMUNICATOR);
  ADIOSUtilities::TestError1(err, adios_get_last_errmsg());

  err = adios_allocate_buffer(ADIOS_BUFFER_ALLOC_NOW, 10);
  ADIOSUtilities::TestError1(err, adios_get_last_errmsg());

  err = adios_declare_group(&this->ADIOSGroup, "VTK", "",
    adios_flag_yes);
  ADIOSUtilities::TestError1(err, adios_get_last_errmsg());

  err = adios_select_method(this->ADIOSGroup, "POSIX", "", "");
  ADIOSUtilities::TestError1(err, adios_get_last_errmsg());
}

//----------------------------------------------------------------------------
ADIOSWriter::~ADIOSWriter(void)
{
  int err;

  if(this->ADIOSFile != ADIOSUtilities::ADIOS_INVALID_INT64)
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
void ADIOSWriter::InitializeFile(const std::string &fileName)
{
  int err;

  err = adios_open(&this->ADIOSFile, "VTK", fileName.c_str(), "w",
    COMMUNICATOR);
  ADIOSUtilities::TestError1(err, adios_get_last_errmsg());

  uint64_t totalSize;
  err = adios_group_size(this->ADIOSFile, this->ADIOSGroupSize,  &totalSize);
  ADIOSUtilities::TestError1(err, adios_get_last_errmsg());
}

//----------------------------------------------------------------------------
void ADIOSWriter::CanDefine(void)
{
  if(this->IsWriting)
    {
    throw std::runtime_error("Unable to declare variables after writing"
      " has started");
    }
}
