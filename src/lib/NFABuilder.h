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

#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include <stack>
#include <list>
#include <stdexcept>
#include <string>

#include "NFA.h"
#include "Token.h"
#include "Lexer.h"
#include "NPIConvertor.h"

template <typename SymbolT>
class NFABuilder
{
private:
  typedef Token<SymbolT> Token;

  NFA<SymbolT>& _nfa;
  std::list<Token> _npi;
  std::stack<NFA<SymbolT>*> _stack;

public:
  NFABuilder(SymbolT const* expr, NFA<SymbolT>& nfa) :
    _nfa(nfa)
  {
    try
    {
      _build(expr);
    }
    catch (std::invalid_argument e)
    {
      _cleanUp();
      throw e;
    }
  }

  NFABuilder(SymbolT const *expr) :
    NFABuilder(expr, *new NFA<SymbolT>)
  {}

  ~NFABuilder()
  {
    _cleanUp();
  }

  NFA<SymbolT>& collect()
  {
    return _nfa;
  }

  NFA<SymbolT> const& collect() const
  {
    return _nfa;
  }

private:
  NFA<SymbolT>& _safePop(std::string const& errMsg="syntax error")
  {
    if (_stack.empty())
    {
      throw std::invalid_argument (errMsg);
    }
    else
    {
      auto& nfa = *_stack.top();
      _stack.pop();
      return nfa;
    }
  }

  NFA<SymbolT>& _starInduction(NFA<SymbolT>& operand) const
  {
    auto initial = operand.getInitial();
    auto in = operand.addState();
    auto out = operand.addState();

    operand.addEpsilonTransition(in, out);
    operand.addEpsilonTransition(in, initial);

    // must be true if the algorithm works correctly
    assert(operand.getAcceptorSet().size() == 1);
    for (auto state : operand.getAcceptorSet())
    {
      operand.addEpsilonTransition(state, initial);
      operand.addEpsilonTransition(state, out);
    }

    operand.replaceInitial(in);
    operand.clearAcceptorSet();
    operand.setAcceptor(out);

    return operand;
  }

  void _treatStar()
  {
    auto& operand = _safePop();
    auto& result = _starInduction(operand);
    _stack.push(&result);
  }

  NFA<SymbolT>& _orInduction(NFA<SymbolT>& left, NFA<SymbolT>& right) const
  {
    auto& result = *new NFA<SymbolT>;
    auto in = result.getInitial();
    auto out = result.addState();
    result.setAcceptor(out);

    result.insert(left, in, out);
    result.insert(right, in, out);

    delete &left;
    delete &right;

    return result;
  }

  void _treatOr()
  {
    auto& rightNFA = _safePop();
    auto& leftNFA = _safePop();

    auto& res = _orInduction(leftNFA, rightNFA);
    _stack.push(&res);
  }

  NFA<SymbolT>& _concatInduction(NFA<SymbolT>& left, NFA<SymbolT>& right) const
  {
    auto newInitial = right.addState();
    right.insert(left, newInitial, right.getInitial());
    right.replaceInitial(newInitial);
    delete &left;
    return right;
  }

  void _treatConcat()
  {
    auto& rightNFA = _safePop();
    auto& leftNFA = _safePop();

    auto& res = _concatInduction(leftNFA, rightNFA);
    _stack.push(&res);
  }

  NFA<SymbolT>& _createSimpleNFA(SymbolT symbol) const
  {
    auto& result = *new NFA<SymbolT>;
    auto out = result.addState();
    result.setAcceptor(out);
    result.addTransition(result.getInitial(), symbol, out);
    return result;
  }

  void _treatLambda(Token token)
  {
    _stack.push(&_createSimpleNFA(token.getValue()));
  }

  void _shunt(Token token)
  {
    switch (token.getLabel())
    {
      case Token::STAR:           _treatStar();         break;
      case Token::OR:             _treatOr();           break;
      case Token::CONCAT:         _treatConcat();       break;
      case Token::LAMBDA:         _treatLambda(token);       break;
      default:
        assert (false); // unreachable, it's a bug otherwise
        throw std::invalid_argument("It's not a bug, it's a feature ... :s");
    }
  }

  void _buildNPI(SymbolT const* expr)
  {
    std::list<Token> tokens;
    Lexer<SymbolT> lexer(expr, tokens);
    NPIConvertor<SymbolT> convertor(tokens, _npi);
  }

  void _buildResult()
  {
    auto acceptor = _nfa.addState();
    _nfa.setAcceptor(acceptor);
    if (_stack.empty())
    {
      _nfa.addEpsilonTransition(_nfa.getInitial(), acceptor);
    }
    else if (_stack.size() == 1)
    {
      auto& lastNFA = *_stack.top();
      _nfa.insert(lastNFA, _nfa.getInitial(), acceptor);
    }
    else
    {
      throw std::invalid_argument("missing operator(s)");
    }
  }

  void _build(SymbolT const* expr)
  {
    _buildNPI(expr);

    for (auto token : _npi)
    {
      _shunt(token);
    }

    _buildResult();
  }

  void _cleanUp()
  {
    while (!_stack.empty())
    {
      delete _stack.top();
      _stack.pop();
    }
  }
};

#endif // NFA_BUILDER_H
