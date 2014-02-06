#include <iostream>
#include <string>
#include <cmath>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataReader.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkTIFFWriter.h>

#include "IO/ADIOS/vtkADIOSImageDataWriter.h"

vtkSmartPointer<vtkImageData> loadImage(const std::string &filename);

void saveLegacyImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename);

void saveADIOSImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename);

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
    image = loadImage(inputFile);
    std::cout << "done" << std::endl;

    image->PrintSelf(std::cout, vtkIndent(4));

    std::cout << "Saving Legacy image data..." << std::flush;
    saveLegacyImage(image, outputFile+".vtk");
    std::cout << "done" << std::endl;

    std::cout << "Saving ADIOS image data..." << std::endl;
    saveADIOSImage(image, outputFile+".bp");
    std::cout << "done" << std::endl;
  }

  return 0;
}

vtkSmartPointer<vtkImageData> loadImage(const std::string &filename)
{
  vtkSmartPointer<vtkXMLImageDataReader> reader =
    vtkSmartPointer<vtkXMLImageDataReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  
  return reader->GetOutput();
}

void saveLegacyImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename)
{
  vtkSmartPointer<vtkStructuredPointsWriter> writer =
    vtkSmartPointer<vtkStructuredPointsWriter>::New();
  writer->SetInputData(image);
  writer->SetFileName(filename.c_str());
  writer->Write();
}

void saveADIOSImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename)
{
  vtkSmartPointer<vtkADIOSImageDataWriter> writer =
    vtkSmartPointer<vtkADIOSImageDataWriter>::New();

  writer->SetFileName(filename);
  writer->SetInput(image);
  writer->Write();
}

