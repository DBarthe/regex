#include "Regex.h"

template<>
template<>
char const* Regex::arrayOfCustom(std::string const&) {
}

template<>
template<>
wchar_t const* WRegex::arrayOfCustom(std::wstring const&) {
}
