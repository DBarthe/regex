#include "Regex.h"
#include "NFABuilder.h"

template<>
template<>
char const* Regex::arrayOfCustom(std::string const&)
{
  return nullptr;
}

template<>
char const NFABuilder<char>::_starSymbol = '*';
template<>
char const NFABuilder<char>::_orSymbol = '|';
template<>
char const NFABuilder<char>::_openParenthSymbol = '(';
template<>
char const NFABuilder<char>::_closeParenthSymbol = ')';
template<>
char const NFABuilder<char>::_endSymbol = '\0';

template<>
template<>
wchar_t const* WRegex::arrayOfCustom(std::wstring const&)
{
  return nullptr;
}

template<>
wchar_t const NFABuilder<wchar_t>::_starSymbol = '*';
template<>
wchar_t const NFABuilder<wchar_t>::_orSymbol = '|';
template<>
wchar_t const NFABuilder<wchar_t>::_openParenthSymbol = '(';
template<>
wchar_t const NFABuilder<wchar_t>::_closeParenthSymbol = ')';
template<>
wchar_t const NFABuilder<wchar_t>::_endSymbol = '\0';
