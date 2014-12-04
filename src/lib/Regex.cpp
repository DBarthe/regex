#include "Regex.h"

template<>
template<>
char const* Regex::arrayOfCustom(std::string const&)
{
  return nullptr;
}

template<>
template<>
wchar_t const* WRegex::arrayOfCustom(std::wstring const&)
{
  return nullptr;
}
