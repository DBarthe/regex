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

#ifndef REGEX_BASE_H
#define REGEX_BASE_H

#include "NFA.h"
#include "NFABuilder.h"
#include "NFASimulator.h"

template <typename SymbolT>
class RegexBase
{
private:
  NFA<SymbolT> _nfa;

public:
  RegexBase(SymbolT const* expr)
  {
    NFABuilder<SymbolT> builder(expr, _nfa);
  }

  template <typename T>
  RegexBase(T const& customExpr) :
    RegexBase(arrayOfCustom(customExpr))
  {}

  ~RegexBase() = default;

  bool match(SymbolT const* input) const
  {
    NFASimulator<SymbolT> simulator;
    return simulator.simulate(_nfa, input);
  }

  template <typename T>
  bool match(T const& customInput) const {
    return match(arrayOfCustom(customInput));
  }

private:
  template <typename T>
  static SymbolT const* arrayOfCustom(T const& custom)
  {
    assert(false /* unimplemented */);
    return nullptr;
  }

};

#endif // REGEX_BASE_H
