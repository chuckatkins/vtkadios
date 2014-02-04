#ifndef _BPUtilitiesVTK_h
#define _BPUtilitiesVTK_h

#include "BPUtilities.h"
#include <adios.h>

class vtkBPUtilities: public BPUtilities
{
public:
  static ADIOS_DATATYPES VTKToADIOSType(int v);
};

#endif
