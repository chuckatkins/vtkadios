#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include "IO/ADIOS/vtkADIOSPolyDataReader.h"

vtkSmartPointer<vtkPolyData> readADIOSPoly(const std::string &filename);

int main(int argc, char **argv)
{
  if(argc != 2)
    {
    std::cerr << "Usage: " << argv[0] << " /path/to/input" << std::endl;
    return 1;
    }

  std::string inputFile(argv[1]);

  {
    vtkSmartPointer<vtkPolyData> poly;

    std::cout << "Reading ADIOS poly data..." << std::endl;
    poly = readADIOSPoly(inputFile);
    if(!poly)
      {
      return 2;
      }
    poly->PrintSelf(std::cout, vtkIndent());
  }

  return 0;
}

vtkSmartPointer<vtkPolyData> readADIOSPoly(const std::string &filename)
{
  vtkNew<vtkADIOSPolyDataReader> reader;

  reader->SetFileName(filename);
  reader->Update();
  reader->PrintSelf(std::cout, vtkIndent());

  return reader->GetOutput();
}
