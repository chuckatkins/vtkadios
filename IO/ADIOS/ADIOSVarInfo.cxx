#include "ADIOSVarInfo.h"
#include "ADIOSUtilities.h"
#include <cstring>
#include <stdexcept>
#include <adios_read.h>

//----------------------------------------------------------------------------
struct ADIOSVarInfo::ADIOSVarInfoImpl
{
  ADIOSVarInfoImpl(const std::string& name = "", ADIOS_VARINFO *var = NULL)
  : Name(name), Var(var)
  { }

  ~ADIOSVarInfoImpl(void)
  {
    if(this->Var)
      {
      adios_free_varinfo(this->Var);
      }
  }

  std::string Name;
  ADIOS_VARINFO *Var;
};

//----------------------------------------------------------------------------
ADIOSVarInfo::ADIOSVarInfo(const std::string& name, void* v)
: Impl(new ADIOSVarInfo::ADIOSVarInfoImpl(name,
    reinterpret_cast<ADIOS_VARINFO*>(v)))
{
}

//----------------------------------------------------------------------------
ADIOSVarInfo::~ADIOSVarInfo(void)
{
  delete this->Impl;
}

//----------------------------------------------------------------------------
std::string ADIOSVarInfo::GetName(void) const
{
  return this->Impl->Name;
}

//----------------------------------------------------------------------------
int ADIOSVarInfo::GetId(void) const
{
  return this->Impl->Var->varid;
}

//----------------------------------------------------------------------------
int ADIOSVarInfo::GetType(void) const
{
  return ADIOSUtilities::TypeADIOSToVTK(this->Impl->Var->type);
}

//----------------------------------------------------------------------------
size_t ADIOSVarInfo::GetNumSteps(void) const
{
  return this->Impl->Var->nsteps;
}

//----------------------------------------------------------------------------
bool ADIOSVarInfo::IsGlobal(void) const
{
  return this->Impl->Var->global == 1;
}

//----------------------------------------------------------------------------
bool ADIOSVarInfo::IsScalar(void) const
{
  return this->Impl->Var->ndim == 0;
}

//----------------------------------------------------------------------------
void ADIOSVarInfo::GetDims(std::vector<size_t>& dims) const
{
  dims.clear();
  dims.insert(dims.begin(), this->Impl->Var->dims,
    this->Impl->Var->dims+this->Impl->Var->ndim);
}

//----------------------------------------------------------------------------
template<typename T>
T ADIOSVarInfo::GetValue(int step) const
{
  if(ADIOSUtilities::TypeNativeToADIOS<T>::T != this->Impl->Var->type)
    {
    throw std::runtime_error("Incompatible type");
    }
  return reinterpret_cast<const T*>(this->Impl->Var->value)[step];
}

template<>
std::string ADIOSVarInfo::GetValue<std::string>(int step) const
{
  if(this->Impl->Var->type != ADIOSUtilities::TypeNativeToADIOS<std::string>::T)
    {
    throw std::runtime_error("Incompatible type");
    }
  return reinterpret_cast<const char**>(this->Impl->Var->value)[step];
}

template int8_t ADIOSVarInfo::GetValue<int8_t>(int) const;
template int16_t ADIOSVarInfo::GetValue<int16_t>(int) const;
template int32_t ADIOSVarInfo::GetValue<int32_t>(int) const;
template int64_t ADIOSVarInfo::GetValue<int64_t>(int) const;
template uint8_t ADIOSVarInfo::GetValue<uint8_t>(int) const;
template uint16_t ADIOSVarInfo::GetValue<uint16_t>(int) const;
template uint32_t ADIOSVarInfo::GetValue<uint32_t>(int) const;
template uint64_t ADIOSVarInfo::GetValue<uint64_t>(int) const;
template vtkIdType ADIOSVarInfo::GetValue<vtkIdType>(int) const;
template float ADIOSVarInfo::GetValue<float>(int) const;
template double ADIOSVarInfo::GetValue<double>(int) const;

//----------------------------------------------------------------------------
template<typename T>
const T* ADIOSVarInfo::GetAllValues(void) const
{
  if(ADIOSUtilities::TypeNativeToADIOS<T>::T != this->Impl->Var->type)
    {
    throw std::runtime_error("Incompatible type");
    }
  return reinterpret_cast<const T*>(this->Impl->Var->value);
}

template const int8_t* ADIOSVarInfo::GetAllValues<int8_t>(void) const;
template const int16_t* ADIOSVarInfo::GetAllValues<int16_t>(void) const;
template const int32_t* ADIOSVarInfo::GetAllValues<int32_t>(void) const;
template const int64_t* ADIOSVarInfo::GetAllValues<int64_t>(void) const;
template const uint8_t* ADIOSVarInfo::GetAllValues<uint8_t>(void) const;
template const uint16_t* ADIOSVarInfo::GetAllValues<uint16_t>(void) const;
template const uint32_t* ADIOSVarInfo::GetAllValues<uint32_t>(void) const;
template const uint64_t* ADIOSVarInfo::GetAllValues<uint64_t>(void) const;
template const vtkIdType* ADIOSVarInfo::GetAllValues<vtkIdType>(void) const;
template const float* ADIOSVarInfo::GetAllValues<float>(void) const;
template const double* ADIOSVarInfo::GetAllValues<double>(void) const;
