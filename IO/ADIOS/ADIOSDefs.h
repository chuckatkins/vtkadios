#ifndef __ADIOSDefs_h
#define __ADIOSDefs_h

#include <string>

namespace ADIOS
{

enum TransportMethod
{
  TransportMethod_NULL          = 0,
  TransportMethod_POSIX         = 1,
  TransportMethod_MPI           = 2,
  TransportMethod_MPI_LUSTRE    = 3,
  TransportMethod_MPI_AGGREGATE = 4,
  TransportMethod_VAR_MERGE     = 5,
  TransportMethod_DataSpaces    = 6,
  TransportMethod_DIMES         = 7,
  TransportMethod_FlexPath      = 8,
  TransportMethod_PHDF5         = 9,
  TransportMethod_NetCDF4       = 10
};
const std::string& ToString(TransportMethod);

enum Transform
{
  Transform_NONE   = 0,
  Transform_ZLIB   = 1,
  Transform_BZLIB2 = 2,
  Transform_SZIP   = 3
};
const std::string& ToString(Transform);

} // end namespace
#endif //__ADIOSDefs_h
