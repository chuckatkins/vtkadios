#include "BPUtilitiesVTK.h"
#include <vtkType.h>
#include <adios.h>

ADIOS_DATATYPES vtkBPUtilities::VTKToADIOSType(int v)
{
  switch(v)
    {
    case VTK_CHAR:
    case VTK_SIGNED_CHAR: return adios_byte;
    case VTK_SHORT: return adios_short;
    case VTK_INT: return adios_integer;
    case VTK_UNSIGNED_CHAR: return adios_unsigned_byte;
    case VTK_UNSIGNED_SHORT: return adios_unsigned_short;
    case VTK_UNSIGNED_INT: return adios_unsigned_integer;
    case VTK_FLOAT: return adios_real;
    case VTK_DOUBLE: return adios_double;
    default: return adios_unknown;
    }
}
