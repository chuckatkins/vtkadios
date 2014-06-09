/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkUnstructuredGeneratorSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkUnstructuredGeneratorSource.h"

#include "UnstructuredGenerator.h"

#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkElevationFilter.h"
#include "vtkExtentTranslator.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnstructuredGrid.h"

vtkStandardNewMacro(vtkUnstructuredGeneratorSource);

//----------------------------------------------------------------------------
vtkUnstructuredGeneratorSource::vtkUnstructuredGeneratorSource()
{
  this->GeneratorMode = GENERATOR_CONTINUOUS;
  this->PyramidStepSize = 20;
  this->SetDataExtent(0, 50, 0, 50, 0, 50);

  this->SetNumberOfInputPorts(0);

  this->Cache = 0;
}

//----------------------------------------------------------------------------
vtkUnstructuredGeneratorSource::~vtkUnstructuredGeneratorSource()
{
  if (this->Cache)
    {
    this->Cache->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkUnstructuredGeneratorSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkUnstructuredGeneratorSource::RequestInformation(
  vtkInformation       * request,
  vtkInformationVector** inputVector,
  vtkInformationVector * outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  if (this->Cache)
    {
    this->Cache->Delete();
    this->Cache = 0;
    }
  // Our source can produce any number of pieces
  outInfo->Set(vtkAlgorithm::CAN_HANDLE_PIECE_REQUEST(), 1);

  static const int NUMBER_OF_STEPS = 20;
  double timeSteps[NUMBER_OF_STEPS];
  for (int i=0; i<NUMBER_OF_STEPS; i++)
    {
    timeSteps[i] = i;
    }
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),
    timeSteps, NUMBER_OF_STEPS);
  double timeRange[2] = {0, NUMBER_OF_STEPS - 1};
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(),
    timeRange, 2);

  return 1;
}

//----------------------------------------------------------------------------
void vtkUnstructuredGeneratorSource::AddTemoralData(
  double time,
  vtkUnstructuredGrid* grid)
{
  vtkNew<vtkElevationFilter> elev;
  elev->SetInputData(grid);
  elev->SetLowPoint(0, 0, 0);
  elev->SetHighPoint(50, 50, 75);
  elev->SetScalarRange(-100, 100);
  elev->Update();

  vtkFloatArray* scalars = vtkFloatArray::SafeDownCast(
    elev->GetOutput()->GetPointData()->GetScalars());

  vtkIdType nvals = scalars->GetNumberOfTuples();
  for (int i=0; i<nvals; i++)
    {
    scalars->SetValue(i, scalars->GetValue(i) * cos(time / 10));
    }

  grid->GetPointData()->AddArray(scalars);
}

//----------------------------------------------------------------------------
int vtkUnstructuredGeneratorSource::RequestData(
  vtkInformation       * request,
  vtkInformationVector** inputVector,
  vtkInformationVector * outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkUnstructuredGrid* grid =
    vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int piece =
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  int numPieces =
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
  double updateTime = 0;
  if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
    {
    updateTime = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
    }

  // Convert unstructured piece to structured extent
  vtkNew<vtkExtentTranslator> translator;
  translator->SetWholeExtent(this->DataExtent);
  translator->SetPiece(piece);
  translator->SetNumberOfPieces(numPieces);
  translator->PieceToExtent();
  int extent[6];
  translator->GetExtent(extent);

  // Fix the overlapping genreated by translator
  for (int i = 0; i < 3; i++)
    {
    if (extent[2*i+1] != this->DataExtent[2*i+1])
      {
      extent[2*i+1]--;
      }
    }

  if (!this->Cache)
    {
    this->Cache = vtkUnstructuredGrid::New();
    switch (this->GeneratorMode)
      {
      case GENERATOR_PILLAR:
        PillarGridGenerator(piece, this->DataExtent, extent, this->Cache);
        break;
      case GENERATOR_DISCONTINOUS:
        DiscontinuousGridGenerator(piece, this->DataExtent, extent, this->Cache);
        break;
      case GENERATOR_STEPS:
        StepsGridGenerator(piece, this->DataExtent, extent, this->Cache, this->PyramidStepSize);
        break;
      case GENERATOR_PYRAMID:
        PyramidGridGenerator(piece, this->DataExtent, extent, this->Cache, this->PyramidStepSize);
        break;   
      case GENERATOR_CONTINUOUS:
      default:
        ContinuousGridGenerator(piece, this->DataExtent, extent, this->Cache);
        break;
      }
    vtkIdType ncells = this->Cache->GetNumberOfCells();
    vtkNew<vtkIntArray> objectId;
    objectId->SetName("ObjectId");
    objectId->SetNumberOfTuples(ncells);
    objectId->FillComponent(0, 0);
    this->Cache->GetCellData()->AddArray(objectId.GetPointer());
    }

  grid->ShallowCopy(this->Cache);
  this->AddTemoralData(updateTime, grid);

  return 1;
}
