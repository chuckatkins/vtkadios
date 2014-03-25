#include <iostream>
#include <string>
#include <cmath>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataReader.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkTIFFWriter.h>

#include "IO/ADIOS/vtkADIOSReader.h"
#include "IO/ADIOS/ADIOSReader.h"

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
    vtkSmartPointer<vtkImageData> image;

    std::cout << "Reading ADIOS image data..." << std::endl;
    image = readADIOSImage(inputFile);
    if(!image)
      {
      return 2;
      }
    image->PrintSelf(std::cout, vtkIndent(4));
  }

  return 0;
}

vtkSmartPointer<vtkImageData> readADIOSImage(const std::string &filename)
{
  vtkNew<vtkADIOSReader> reader;
  reader->SetFileName(filename);
  reader->Read();

  reader->PrintSelf(std::cout, vtkIndent(1));

  return NULL;
}
