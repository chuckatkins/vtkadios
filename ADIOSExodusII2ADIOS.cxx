#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkMPIController.h>
#include <vtkMultiProcessController.h>

#include <vtkPExodusIIReader.h>
#include "Extra/vtkCompositeDataToUnstructuredGridFilter.h"
#include "IO/ADIOS/vtkADIOSWriter.h"

vtkSmartPointer<vtkImageData> readVTKImage(const std::string &filename);
void writeADIOSImage(vtkSmartPointer<vtkImageData>, const std::string&);

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

  vtkNew<vtkPExodusIIReader> reader;
  vtkNew<vtkCompositeDataToUnstructuredGridFilter> filter;
  vtkNew<vtkADIOSWriter> writer;

  filter->SetInputConnection(reader->GetOutputPort());
  writer->SetInputConnection(filter->GetOutputPort());

  reader->SetFileName(argv[1]);
  reader->UpdateInformation();
  for(int i = 0; i < reader->GetNumberOfElementResultArrays(); ++i)
    {
    reader->SetElementResultArrayStatus(reader->GetElementResultArrayName(i),
      1);
    }

  writer->SetFileName(argv[2]);
  writer->SetTransform("zlib:1");
  writer->Update();

  return 0;
}
