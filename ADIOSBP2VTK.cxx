#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkMPIController.h>
#include <vtkMultiProcessController.h>

#include <vtkADIOSReader.h>
#include <vtkPExodusIIWriter.h>
#include "Extra/vtkCompositeDataToUnstructuredGridFilter.h"

int main(int argc, char **argv)
{
  vtkNew<vtkMPIController> controller;
  controller->Initialize(&argc, &argv, 0);
  vtkMultiProcessController::SetGlobalController(controller.GetPointer());

  if(argc != 3)
    {
    std::cout << "Usage: " << argv[0] << " /path/to/input /path/to/output\n";
    return 1;
    }

  std::string inputFile(argv[1]),
              outputFile(argv[2]);

  vtkNew<vtkADIOSReader> reader;
  vtkNew<vtkCompositeDataToUnstructuredGridFilter> filter;
  vtkNew<vtkPExodusIIWriter> writer;

  reader->SetFileName(argv[1]);
  filter->SetInputConnection(reader->GetOutputPort());
  writer->SetFileName(argv[2]);
  writer->SetWriteAllTimeSteps(1);
  writer->SetInputConnection(filter->GetOutputPort());

  writer->Update();

  return 0;
}
