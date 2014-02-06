#ifndef _ADIOSUtilitiesVTK_h
#define _ADIOSUtilitiesVTK_h

#include "ADIOSUtilities.h"
#include <adios.h>

class vtkADIOSUtilities: public ADIOSUtilities
{
public:
  static ADIOS_DATATYPES VTKToADIOSType(int v);
};

#endif
