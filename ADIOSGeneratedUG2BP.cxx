#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkMPIController.h>
#include <vtkMultiProcessController.h>

#include <vtkPExodusIIReader.h>
#include <vtkADIOSWriter.h>
#include "Extra/vtkUnstructuredGeneratorSource.h"

int main(int argc, char **argv)
{
  vtkNew<vtkMPIController> controller;
  controller->Initialize(&argc, &argv, 0);
  vtkMultiProcessController::SetGlobalController(controller.GetPointer());

  if(argc != 2)
    {
    std::cout << "Usage: " << argv[0] << " /path/to/output\n";
    return 1;
    }

  std::string outputFile(argv[1]);

  vtkNew<vtkUnstructuredGeneratorSource> source;
  vtkNew<vtkADIOSWriter> writer;

  writer->SetInputConnection(source->GetOutputPort());

  source->SetGeneratorMode(0);
  source->UpdateInformation();

  writer->SetFileName(outputFile.c_str());
  //writer->SetTransformToZLib();
  writer->Update();

  return 0;
}
