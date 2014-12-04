#ifndef REGEX_H
#define REGEX_H

#include <string>

#include "RegexBase.h"

typedef RegexBase<char> Regex;
typedef RegexBase<wchar_t> WRegex;

template<>
template<>
char const* Regex::arrayOfCustom(std::string const&);

template<>
template<>
wchar_t const* WRegex::arrayOfCustom(std::wstring const&);

#endif // REGEX_H
