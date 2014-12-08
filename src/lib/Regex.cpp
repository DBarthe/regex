// The MIT License (MIT)

// Copyright (c) 2014 Barthelemy Delemotte

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "Regex.h"
#include "Lexemes.h"

template<>
template<>
char const* Regex::arrayOfCustom(std::string const& str)
{
  return str.c_str();
}

template<>
char const Lexemes<char>::STAR = '*';
template<>
char const Lexemes<char>::OR = '|';
template<>
char const Lexemes<char>::PLUS = '+';
template<>
char const Lexemes<char>::LEFT_PARENTH = '(';
template<>
char const Lexemes<char>::RIGHT_PARENTH = ')';
template<>
char const Lexemes<char>::END = '\0';

template<>
std::string Lexemes<char>::toString(char sym)
{
  return std::string(1, sym);
}

template<>
template<>
wchar_t const* WRegex::arrayOfCustom(std::wstring const& str)
{
  return str.c_str();
}

template<>
wchar_t const Lexemes<wchar_t>::STAR = '*';
template<>
wchar_t const Lexemes<wchar_t>::OR = '|';
template<>
wchar_t const Lexemes<wchar_t>::PLUS = '+';
template<>
wchar_t const Lexemes<wchar_t>::LEFT_PARENTH = '(';
template<>
wchar_t const Lexemes<wchar_t>::RIGHT_PARENTH = ')';
template<>
wchar_t const Lexemes<wchar_t>::END = '\0';
