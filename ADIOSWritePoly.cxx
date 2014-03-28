#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkStructuredPointsWriter.h>

#include "IO/ADIOS/vtkADIOSWriter.h"

vtkSmartPointer<vtkPolyData> readVTKPoly(const std::string &filename);
void writeADIOSPoly(vtkSmartPointer<vtkPolyData>, const std::string&);

int main(int argc, char **argv)
{
  if(argc != 3)
    {
    std::cout << "Usage: " << argv[0] << " /path/to/input /path/to/output\n";
    return 1;
    }

  std::string inputFile(argv[1]),
              outputFile(argv[2]);

  {
    vtkSmartPointer<vtkPolyData> poly;

    std::cout << "Loading poly data..." << std::flush;
    poly = readVTKPoly(inputFile);
    std::cout << "done" << std::endl;

    poly->PrintSelf(std::cout, vtkIndent(4));

    std::cout << "Saving ADIOS poly data..." << std::endl;
    writeADIOSPoly(poly, outputFile);
    std::cout << "done" << std::endl;
  }

  return 0;
}

vtkSmartPointer<vtkPolyData> readVTKPoly(const std::string &filename)
{
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();
  
  return reader->GetOutput();
}

void writeADIOSPoly(
  const vtkSmartPointer<vtkPolyData> poly,
  const std::string &filename)
{
  //vtkNew<vtkADIOSPolyDataWriter> writer;

  //writer->SetFileName(filename);
  //writer->SetInput(poly);
  //writer->Write();

  vtkNew<vtkADIOSWriter> writer;

  writer->SetFileName(filename);
  writer->Define("", poly);
  writer->InitializeFile();
  writer->Write("", poly);
}
