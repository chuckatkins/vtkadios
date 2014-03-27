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
void ADIOSVarInfo::GetDims(std::vector<size_t>& dims) const
{
  dims.clear();
  dims.insert(dims.begin(), this->Impl->Var->dims,
    this->Impl->Var->dims+this->Impl->Var->ndim);
}

//----------------------------------------------------------------------------
bool ADIOSVarInfo::IsGlobal(void) const
{
  return this->Impl->Var->global == 1;
}

//----------------------------------------------------------------------------
template<typename T>
T ADIOSVarInfo::GetValue(void) const
{
  #define CAST_RETURN(TN) return static_cast<T>(reinterpret_cast<const T*>(this->Impl->Var->value));

  switch(this->Impl->Var->type)
    {
    case adios_byte: CAST_RETURN(int8_t)
    throw std::runtime_error("Incompatible type");
    }
}

template<>
std::string ADIOSVarInfo::GetValue<std::string>(void) const
{
  if(this->Impl->Var->type != ADIOSUtilities::TypeNativeToADIOS<std::string>::T)
    {
    throw std::runtime_error("Incompatible type");
    }
  return reinterpret_cast<const char*>(this->Impl->Var->value);
}

template<> int8_t ADIOSVarInfo::GetValue<int8_t>(void) const;
template<> int16_t ADIOSVarInfo::GetValue<int16_t>(void) const;
template<> int32_t ADIOSVarInfo::GetValue<int32_t>(void) const;
template<> uint8_t ADIOSVarInfo::GetValue<uint8_t>(void) const;
template<> uint16_t ADIOSVarInfo::GetValue<uint16_t>(void) const;
template<> uint32_t ADIOSVarInfo::GetValue<uint32_t>(void) const;
template<> float ADIOSVarInfo::GetValue<float>(void) const;
template<> double ADIOSVarInfo::GetValue<double>(void) const;