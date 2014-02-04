#include "BPUtilities.h"
#include <stdint.h>
#include <limits>
#include <complex>
#include <string>

#include <adios.h>

template<> ADIOS_DATATYPES BPUtilities::BPType<int8_t>::T = adios_byte;
template<> ADIOS_DATATYPES BPUtilities::BPType<int16_t>::T = adios_short;
template<> ADIOS_DATATYPES BPUtilities::BPType<int32_t>::T = adios_integer;
template<> ADIOS_DATATYPES BPUtilities::BPType<int64_t>::T = adios_long;
template<> ADIOS_DATATYPES BPUtilities::BPType<uint8_t>::T = adios_unsigned_byte;
template<> ADIOS_DATATYPES BPUtilities::BPType<uint16_t>::T = adios_unsigned_short;
template<> ADIOS_DATATYPES BPUtilities::BPType<uint32_t>::T = adios_unsigned_integer;
template<> ADIOS_DATATYPES BPUtilities::BPType<uint64_t>::T = adios_unsigned_long;
template<> ADIOS_DATATYPES BPUtilities::BPType<float>::T = adios_real;
template<> ADIOS_DATATYPES BPUtilities::BPType<double>::T = adios_double;
template<> ADIOS_DATATYPES BPUtilities::BPType<long double>::T = adios_long_double;
template<> ADIOS_DATATYPES BPUtilities::BPType<std::complex<float> >::T = adios_complex;
template<> ADIOS_DATATYPES BPUtilities::BPType<std::complex<double> >::T = adios_double_complex;
template<> ADIOS_DATATYPES BPUtilities::BPType<long long>::T = adios_long;
template<> ADIOS_DATATYPES BPUtilities::BPType<std::string>::T = adios_string;

const int64_t BPUtilities::ADIOS_INVALID_INT64 = std::numeric_limits<int64_t>::max();

size_t BPUtilities::TypeSize(ADIOS_DATATYPES t)
{
  switch(t)
    {
    case adios_byte:
    case adios_unsigned_byte: return 1;
    case adios_short:
    case adios_unsigned_short: return 2;
    case adios_integer:
    case adios_unsigned_integer:
    case adios_real: return 4;
    case adios_long:
    case adios_unsigned_long:
    case adios_double:
    case adios_complex:  return 8;
    case adios_long_double:
    case adios_double_complex: return 16;
    default: return 0;
    }
}

// Specialization for std::string
template<>
size_t BPUtilities::Define<std::string>(int64_t group, const std::string &path,
  const std::string &data)
{
  std::cout << "Def: " << path << " " << data.size() << std::endl;
  int id = adios_define_var(group, path.c_str(), "", adios_string,
    NULL, NULL, NULL);
  TestError0(id, "Failed to define "+path);
  return data.size();
}

// Specialization for std::string; automatically convert to char*
template<>
void BPUtilities::Write<std::string>(int64_t file, const std::string &path,
  const std::string &data)
{
  int err = adios_write(file, path.c_str(), const_cast<char*>(data.c_str()));
  TestError1(err, "Failed to write "+path);
}
