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
#include <stdexcept>

#include "NFA.h"
#include "Token.h"
#include "Lexer.h"

template <typename SymbolT>
class NFABuilder
{
private:
  typedef Token<SymbolT> Token;

  NFA<SymbolT> _nfa;

  struct Workspace
  {
    ~Workspace()
    {
      while (!nfaStack.empty())
      {
        delete nfaStack.top();
        nfaStack.pop();
      }
    }

    std::stack<Token> operatorStack;
    std::stack<NFA<SymbolT>*> nfaStack;
    std::vector<Token> tokens;
    size_t index;
  };

  Workspace* _workspace;

private:
  NFA<SymbolT>& _popNFA()
  {
    if (_workspace->nfaStack.empty())
    {
      throw std::invalid_argument ("NFABuilder: syntax error");
    }
    else
    {
      NFA<SymbolT>* nfa = _workspace->nfaStack.top();
      _workspace->nfaStack.pop();
      return *nfa;
    }
  }

  void _pushNFA(NFA<SymbolT>& nfa)
  {
    _workspace->nfaStack.push(&nfa);
  }

  Token _popOperator()
  {
    if (_workspace->operatorStack.empty())
    {
      throw std::invalid_argument ("NFABuilder: syntax error");
    }
    else
    {
      Token op = _workspace->operatorStack.top();
      _workspace->operatorStack.pop();
      return op;
    }
  }

  void _pushOperator(Token op)
  {
    _treatStackedOperators(
      [this]()
      {

        return Token::END;

      });
    _workspace->operatorStack.push(op);
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

  NFA<SymbolT>& _concatInduction(NFA<SymbolT>& left, NFA<SymbolT>& right)
  {
    auto newInitial = right.addState();
    right.insert(left, newInitial, right.getInitial());
    right.replaceInitial(newInitial);
    delete &left;
    return right;
  }

  NFA<SymbolT>& _baseCase(SymbolT symbol) const
  {
    auto& result = *new NFA<SymbolT>;
    auto out = result.addState();
    result.setAcceptor(out);
    result.addTransition(result.getInitial(), symbol, out);
    return result;
  }

  void _treatStar()
  {
    NFA<SymbolT>& operand = _popNFA();
    NFA<SymbolT>& result = _starInduction(operand);
    _pushNFA(result);
  }

  void _treatOr()
  {
    _pushOperator(Token(Token::OR));
  }

  void _treatOpenParenth()
  {
    _pushOperator(Token(Token::LEFT_PARENTH));
  }

  void _treatStackedOperators(
    std::function<typename Token::Label ()> succFunc)
  {
    typename Token::Label label;

    while ((label = succFunc()) != Token::END)
    {
      switch (label)
      {
        case Token::OR:
        {
          NFA<SymbolT>& rOperand = _popNFA();
          NFA<SymbolT>& lOperand = _popNFA();
          NFA<SymbolT>& result = _orInduction(lOperand, rOperand);
          _pushNFA(result);
        }
        break;
  
        case Token::CONCAT:
        {
          NFA<SymbolT>& rOperand = _popNFA();
          NFA<SymbolT>& lOperand = _popNFA();
          NFA<SymbolT>& result = _concatInduction(lOperand, rOperand);
          _pushNFA(result);
        }
        break;

        default:
          throw std::invalid_argument ("NFABuilder: syntax error");
      }
    }
  }

  void _treatCloseParenth()
  {
    _treatStackedOperators(
      [this]() {
        Token op = _popOperator();
        return op.getLabel() == Token::LEFT_PARENTH
          ? Token::END
          : op.getLabel();
      });
  }

  void _treatConcat() {}

  void _treatLambda()
  {
    NFA<SymbolT>& simpleNFA = _baseCase(_current().getValue());
    _pushNFA(simpleNFA);
  }

  Token _current() const
  {
    return _workspace->tokens[_workspace->index];
  }

  Token _next() const
  {
    return _ended()
      ? _current()
      :  _workspace->tokens[_workspace->index + 1];
  }

  bool _ended() const
  {
    return _current().getLabel() == Token::END;
  }

  void _forward()
  {
    _workspace->index++;
  }

  void _start()
  {
    _workspace->index = 0;
  }

  void _shunt()
  {
    switch (_current().getLabel())
    {
      case Token::STAR:           _treatStar();         break;
      case Token::OR:             _treatOr();           break;
      case Token::LEFT_PARENTH:   _treatOpenParenth();  break;
      case Token::RIGHT_PARENTH:  _treatCloseParenth(); break;
      case Token::CONCAT:         _treatConcat();       break;
      case Token::LAMBDA:         _treatLambda();       break;
      default:;
    }
  }

  void _unloadOperatorStack()
  {
    _treatStackedOperators(
      [this]() {
        if (_workspace->operatorStack.empty())
        {
          return Token::END;
        }
        else
        {
          return _popOperator().getLabel();
        }
      });
  }

  void _finalNFAConcat()
  {
    _nfa.setAcceptor(_nfa.getInitial());
    while (!_workspace->nfaStack.empty())
    {
      auto& current = _popNFA();
      auto newInitial = _nfa.addState();
      _nfa.insert(current, newInitial, _nfa.getInitial());
      _nfa.replaceInitial(newInitial);
      delete &current;
    }
  }


  void _tokenize(SymbolT const* expr)
  {
    Lexer<SymbolT> lexer(expr);
    lexer.tokenize();
    _workspace->tokens.reserve(lexer.collect().size());
    for (auto token : lexer.collect())
    {
      _workspace->tokens.push_back(token);
    }
  }

  void _build(SymbolT const* expr)
  {
    _tokenize(expr);
    for (_start(); !_ended(); _forward())
    {
      _shunt();
    }
    _unloadOperatorStack();
    _finalNFAConcat();
  }

public:
  NFABuilder(SymbolT const* expr) :
    _nfa(), _workspace(new Workspace())
  {
    try
    {
      _build(expr);
      delete _workspace;
      _workspace = nullptr;
    }
    catch (std::invalid_argument e)
    {
      delete _workspace;
      throw e;
    }
  }

  ~NFABuilder() = default;

  NFA<SymbolT>& collect()
  {
    return _nfa;
  }

  NFA<SymbolT> const& collect() const
  {
    return _nfa;
  }
};

#endif // NFA_BUILDER_H
