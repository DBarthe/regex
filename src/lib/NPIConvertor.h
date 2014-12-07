// The MIT License (MIT)

// Copyright (c)2014 Delemotte Barthelemy

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

#ifndef NPI_CONVERTOR_H
#define NPI_CONVERTOR_H

#include <stack>
#include <list>
#include <vector>
#include <set>

#include "Token.h"

template <typename SymbolT>
class NPIConvertor
{
private:
  typedef Token<SymbolT> Token;

  enum Fixity { LEFT, RIGHT, BOTH };

public:
  NPIConvertor(std::list<Token> const& input, std::list<Token>& output) :
    _input(input), _output(output)
  {
    _convert();
  }

  NPIConvertor(std::list<Token> const& input) :
    NPIConvertor(input, *new std::list<Token>)
  {}

  std::list<Token>& collect()
  {
    return _output;
  }

  std::list<Token> const& collect() const
  {
    return _output;
  }

private:
  std::list<Token> const& _input;
  std::list<Token>& _output;
  std::stack<Token> _stack;

private:
  static int _getPrecedence(typename Token::Label o)
  {
    // first = lower precedence, last = higher precedence
    static const std::vector<std::set<typename Token::Label>> table {
      { Token::CONCAT },
      { Token::OR },
      { Token::STAR }
    };

    size_t res = 0;
    for (auto const& set : table)
    {
      if (set.count(o) == 1)
      {
        return res;
      }
      else
      {
        res++;
      }
    }
    throw std::invalid_argument("unkown operator");
  }

  static int _comparePrecedence(typename Token::Label o1, typename Token::Label o2)
  {
    return _getPrecedence(o1) - _getPrecedence(o2);
  }

  static Fixity _getFixity(typename Token::Label o)
  {
    static std::set<typename Token::Label> rightSet {
      Token::CONCAT, Token::OR, Token::STAR
    };
    static std::set<typename Token::Label> leftSet {
      Token::CONCAT, Token::OR, Token::STAR
    };

    bool r = rightSet.count(o) == 1;
    bool l = leftSet.count(o) == 1;
 
    if (r && l)
    {
      return BOTH;
    }
    else if (r)
    {
      return RIGHT;
    }
    else // if (l)
    {
      return LEFT;
    }
  }

  void _treatRightParenthesis()
  {
    while (!_stack.empty() && _stack.top().getLabel() != Token::LEFT_PARENTH)
    {
      _output.push_back(_stack.top());
      _stack.pop();
    }
    if (_stack.empty())
    {
      throw std::invalid_argument ("missing left parenthesis");
    }
    else
    {
      _stack.pop();
    }
  }

  void _treatOperator(Token op)
  {
    Fixity fixity = _getFixity(op.getLabel());
    bool stop = false;
    while (!stop && !_stack.empty())
    {
      Token topOp = _stack.top();

      if (topOp.getLabel() == Token::LEFT_PARENTH)
      {
        stop = true;
      }
      else
      {
        int cmpPrec = _comparePrecedence(op.getLabel(), topOp.getLabel());
        if (((fixity == BOTH || fixity == LEFT) && cmpPrec <= 0)
          || (fixity == RIGHT && cmpPrec < 0))
        {
          _output.push_back(topOp);
          _stack.pop();
        }
        else
        {
          stop = true;
        }
      }
    }
    _stack.push(op);
  }

  void _shunt(Token token)
  {
    switch (token.getLabel())
    {
      case Token::LAMBDA:
        _output.push_back(token);
        break;

      case Token::LEFT_PARENTH:
        _stack.push(token);
        break;

      case Token::RIGHT_PARENTH:
        _treatRightParenthesis();
        break;

      // operator
      default:
        _treatOperator(token);
        break;
    }
  }

  void _unloadStack()
  {
    while (!_stack.empty())
    {
      Token topOp = _stack.top();
      _stack.pop();
      if (topOp.getLabel() == Token::LEFT_PARENTH)
      {
        throw std::invalid_argument ("missing right parenthesis");
      }
      else
      {
        _output.push_back(topOp);
      }
    }
  }

  void _convert()
  {
    for (auto token : _input)
    {
      if (token.getLabel() == Token::END)
      {
        break;
      }
      else
      {
        _shunt(token);
      }
    }
    _unloadStack();
  }
};

#endif // NPI_CONVERTOR_H
