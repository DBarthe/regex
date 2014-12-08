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

#ifndef LEXER_H
#define LEXER_H

#include <map>

#include "Lexemes.h"
#include "Token.h"

template <typename SymbolT>
class Lexer
{
private:
  typedef Token<SymbolT> Token;

  SymbolT const* _input;
  std::list<Token>& _tokenList;
  size_t _index = 0;
  
public:
  Lexer(SymbolT const* input, std::list<Token>& output) :
    _input(input), _tokenList(output)
  {
    _tokenize();
  }

  Lexer(SymbolT const* input) :
    Lexer(input, *new std::list<Token>)
  {}

  std::list<Token> const& collect() const
  {
    return _tokenList;
  }

  std::list<Token>& collect()
  {
    return _tokenList;
  }

private:
  SymbolT _current() const
  {
    return _input[_index];
  }

  SymbolT _peek() const
  {
    if (_current() != Lexemes<SymbolT>::END)
    {
      return _input[_index + 1];
    }
    else
    {
      return Lexemes<SymbolT>::END;
    }
  }

  SymbolT _next()
  {
    if (_current() != Lexemes<SymbolT>::END)
    {
      _index++;
    }
    return _current();
  }

  void _product(Token token)
  {
    _tokenList.push_back(token);
  }

  void _product(typename Token::Label label)
  {
    _tokenList.emplace_back(label);
  }

  void _product(typename Token::Label label, SymbolT value)
  {
    _tokenList.emplace_back(label, value);
  }

  void _maybeAddConcat()
  {
    static const std::set<typename Token::Label> concerned {
      Token::LAMBDA, Token::STAR, Token::RIGHT_PARENTH,
      Token::PLUS, Token::OPTION
    };
    typename Token::Label label = _tokenList.back().getLabel();

    if (concerned.count(label) == 1)
    { 
      _product(Token::CONCAT);
    }
  }

  void _tokenizeCurrent()
  {
    static const std::map<SymbolT, typename Token::Label> table {
      { Lexemes<SymbolT>::STAR, Token::STAR },
      { Lexemes<SymbolT>::OR, Token::OR },
      { Lexemes<SymbolT>::PLUS, Token::PLUS },
      { Lexemes<SymbolT>::LEFT_PARENTH, Token::LEFT_PARENTH },
      { Lexemes<SymbolT>::RIGHT_PARENTH, Token::RIGHT_PARENTH },
      { Lexemes<SymbolT>::OPTION, Token::OPTION },
    };

    SymbolT sym = _current();

    if (sym == Lexemes<SymbolT>::LEFT_PARENTH)
    {
      _maybeAddConcat();
    }

    auto const& it = table.find(sym);
    if (it != table.end())
    {
      _product(it->second);
    }
    else
    {
      _maybeAddConcat();
      _product(Token::LAMBDA, sym);
    }
  }

  void _tokenize()
  {
    while (_current() != Lexemes<SymbolT>::END)
    {
      _tokenizeCurrent();
      _next();
    }
    _product(Token::END);
  }
};

#endif // LEXER_H
