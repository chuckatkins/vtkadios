#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkDataArray.h>

#include "IO/ADIOS/vtkADIOSReader.h"
#include "IO/ADIOS/ADIOSReader.h"

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
    poly->PrintSelf(std::cout, vtkIndent(4));
  }

  return 0;
}

vtkSmartPointer<vtkPolyData> readADIOSPoly(const std::string &filename)
{
  vtkNew<vtkADIOSReader> reader;
  reader->SetFileName(filename);
  reader->OpenAndReadMetadata();
  reader->PrintSelf(std::cout, vtkIndent(1));

  vtkSmartPointer<vtkPolyData> data(reader->ReadObject<vtkPolyData>("/"));
  reader->WaitForReads();

  return data;
}
