#ifndef __ADIOSUtilities_h
#define __ADIOSUtilities_h

#include <stdint.h>

#include <limits>
#include <stdexcept>
#include <sstream>

#include <adios.h>

#include <iostream>

class ADIOSUtilities
{
private:
  static inline std::string ToString(size_t s)
  {
    std::stringstream ss;
    ss << s;
    return ss.str();
  }

public:

  // Description:
  // Map C and C++ primitive datatypes into ADIOS datatypes
  // Specialization defined after class definition
  template<typename U>
  struct ADIOSType
  {
    static ADIOS_DATATYPES T;
  };

  static const int64_t ADIOS_INVALID_INT64;

  static inline void TestError0(int err, const std::string &msg)
  { if(err == 0) { throw std::runtime_error((msg)); } }

  static inline void TestError1(int err, const std::string &msg)
  { if(err != 0) { throw std::runtime_error((msg)); } }

  static size_t TypeSize(ADIOS_DATATYPES t);

  template<typename U>
  static inline size_t Define(int64_t group, const std::string &path,
    size_t len = 1)
  {
    Define(group, path, len == 1 ? "" : ToString(len), ADIOSType<U>::T);
    return len * sizeof(U);
  }

  template<typename U>
  static inline size_t Define(int64_t group, const std::string &path,
    const std::string &len)
  {
    return Define(group, path, len, ADIOSType<U>::T);
  }

  static inline size_t Define(int64_t group, const std::string &path,
    ADIOS_DATATYPES t)
  {
    return Define(group, path, "", t);
  }

  static inline size_t Define(int64_t group, const std::string &path,
    const std::string &dim, ADIOS_DATATYPES t)
  {
    std::cout << "Def: " << path << " " << dim << std::endl;
    int id = adios_define_var(group, path.c_str(), "", t, 
      dim.empty() ? NULL : dim.c_str(), NULL, NULL);
    TestError0(id, "Failed to define "+path);
    return TypeSize(t);
  }

  template<typename T>
  static void Write(int64_t file, const std::string &path, const T &data)
  {
    Write<T>(file, path, &data);
  }

  template<typename T>
  static void Write(int64_t file, const std::string &path, const T *data)
  {
    std::cout << "Write: " << path << std::endl;
    int err = adios_write(file, path.c_str(), const_cast<T*>(data));
    TestError1(err, "Failed to write "+path);
  }

};

#endif
