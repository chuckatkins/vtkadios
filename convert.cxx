#include <iostream>
#include <string>
#include <cmath>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageWriter.h>
#include <vtkTIFFWriter.h>

#include "IO/BP/vtkBPImageDataWriter.h"

vtkSmartPointer<vtkImageData> loadImage(const std::string &filename);

void saveTIFFImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename);

void saveBPImage(
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

    std::cout << "Saving BP image data..." << std::flush;
    saveBPImage(image, outputFile);
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

void saveTIFFImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename)
{
  vtkSmartPointer<vtkTIFFWriter> writer =
    vtkSmartPointer<vtkTIFFWriter>::New();
  writer->SetInputData(image);
  writer->SetFileName(filename.c_str());
  writer->Write();
}

void saveBPImage(
  const vtkSmartPointer<vtkImageData> image,
  const std::string &filename)
{
  vtkSmartPointer<vtkBPImageDataWriter> writer =
    vtkSmartPointer<vtkBPImageDataWriter>::New();

  writer->SetFileName(filename);
  writer->SetInput(image);
  writer->Write();
}

