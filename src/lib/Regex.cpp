#include "Regex.h"
#include "Lexemes.h"

template<>
template<>
char const* Regex::arrayOfCustom(std::string const&)
{
  return nullptr;
}

template<>
char const Lexemes<char>::starSymbol = '*';
template<>
char const Lexemes<char>::orSymbol = '|';
template<>
char const Lexemes<char>::openParenthSymbol = '(';
template<>
char const Lexemes<char>::closeParenthSymbol = ')';
template<>
char const Lexemes<char>::endSymbol = '\0';

template<>
template<>
wchar_t const* WRegex::arrayOfCustom(std::wstring const&)
{
  return nullptr;
}

template<>
wchar_t const Lexemes<wchar_t>::starSymbol = '*';
template<>
wchar_t const Lexemes<wchar_t>::orSymbol = '|';
template<>
wchar_t const Lexemes<wchar_t>::openParenthSymbol = '(';
template<>
wchar_t const Lexemes<wchar_t>::closeParenthSymbol = ')';
template<>
wchar_t const Lexemes<wchar_t>::endSymbol = '\0';
