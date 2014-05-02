#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataReader.h>
#include <vtkStructuredPointsWriter.h>

#include "IO/ADIOS/vtkADIOSWriter.h"

vtkSmartPointer<vtkImageData> readVTKImage(const std::string &filename);
void writeADIOSImage(vtkSmartPointer<vtkImageData>, const std::string&);

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
    vtkSmartPointer<vtkImageData> image;

    std::cout << "Loading image data..." << std::flush;
    image = readVTKImage(inputFile);
    std::cout << "done" << std::endl;

    image->PrintSelf(std::cout, vtkIndent(4));

    std::cout << "Saving ADIOS image data..." << std::endl;
    writeADIOSImage(image, outputFile);
    std::cout << "done" << std::endl;
  }

  return 0;
}

vtkSmartPointer<vtkImageData> readVTKImage(const std::string &filename)
{
  vtkNew<vtkXMLImageDataReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();
  
  return reader->GetOutput();
}

void writeADIOSImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename)
{
  //vtkNew<vtkADIOSImageDataWriter> writer;

  //writer->SetFileName(filename);
  //writer->SetInput(image);
  //writer->Write();

  vtkNew<vtkADIOSWriter> writer;

  writer->SetFileName(filename);
  writer->Define("", image);

  writer->OpenFile();
  writer->Write("", image);
  writer->CloseFile();

  writer->OpenFile();
  writer->Write("", image);
  writer->CloseFile();

  writer->OpenFile();
  writer->Write("", image);
  writer->CloseFile();
}
