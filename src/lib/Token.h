// The MIT License (MIT)

// Copyright (c) 2014 Delemotte Barthelemy

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

#ifndef TOKEN_H
#define TOKEN_H

#include <list>

#include "Lexemes.h"

template <typename SymbolT>
class Token
{
public:
  enum Label
  {
    STAR,
    OR,
    LEFT_PARENTH,
    RIGHT_PARENTH,
    CONCAT,
    OPTION,
    PLUS,
    END,
    LAMBDA
  };

private:
  Label _label;
  SymbolT _value;

public:
  Token(Label label, SymbolT value=Lexemes<SymbolT>::END) :
    _label(label), _value(value)
  {}

  Label getLabel() const
  {
    return _label;
  }

  SymbolT getValue() const
  {
    return _value;
  }

  std::string toString() const
  {
    switch (_label)
    {
      case STAR: return "*";
      case OR: return "|";
      case PLUS: return "+";
      case OPTION: return "?";
      case LEFT_PARENTH: return "(";
      case RIGHT_PARENTH: return ")";
      case CONCAT: return ".";
      case END: return "[END]";
      case LAMBDA: return Lexemes<SymbolT>::toString(_value);
    }
  }
};

#endif // TOKEN_H
