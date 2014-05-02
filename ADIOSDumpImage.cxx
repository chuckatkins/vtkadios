#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "IO/ADIOS/vtkADIOSImageDataReader.h"

vtkSmartPointer<vtkImageData> readADIOSImage(const std::string &filename);

int main(int argc, char **argv)
{
  if(argc != 2)
    {
    std::cerr << "Usage: " << argv[0] << " /path/to/input" << std::endl;
    return 1;
    }

  std::string inputFile(argv[1]);

  {
    vtkSmartPointer<vtkImageData> poly;

    std::cout << "Reading ADIOS poly data..." << std::endl;
    poly = readADIOSImage(inputFile);
    if(!poly)
      {
      return 2;
      }
    poly->PrintSelf(std::cout, vtkIndent(4));
  }

  return 0;
}

vtkSmartPointer<vtkImageData> readADIOSImage(const std::string &filename)
{
  vtkNew<vtkADIOSImageDataReader> reader;

  reader->SetFileName(filename);
  reader->Update();

  return reader->GetOutput();
}
