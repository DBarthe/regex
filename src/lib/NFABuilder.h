#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include <stack>
#include <stdexcept>

#include "NFA.h"

template <typename SymbolT>
class NFABuilder
{
private:
  NFA<SymbolT> _nfa;

  struct Workspace
  {
    Workspace(SymbolT const* _expr) : expr(_expr) {}
    ~Workspace()
    {
      while (!nfaStack.empty())
      {
        delete nfaStack.top();
        nfaStack.pop();
      }
    }

    std::stack<SymbolT> operatorStack;
    std::stack<NFA<SymbolT>*> nfaStack;
    SymbolT const* expr;
    size_t index;
  };

  Workspace* _workspace;

  static SymbolT const _starSymbol;
  static SymbolT const _orSymbol;
  static SymbolT const _openParenthSymbol;
  static SymbolT const _closeParenthSymbol;
  static SymbolT const _endSymbol;

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

  SymbolT _popOperator()
  {
    if (_workspace->operatorStack.empty())
    {
      throw std::invalid_argument ("NFABuilder: syntax error");
    }
    else
    {
      SymbolT op = _workspace->operatorStack.top();
      _workspace->operatorStack.pop();
      return op;
    }
  }

  void _pushOperator(SymbolT op)
  {
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
    _pushOperator(_orSymbol);
  }

  void _treatOpenParenth()
  {
    _pushOperator(_openParenthSymbol);
  }


  void _treatStackedOperators(
    std::function<SymbolT ()> succFunc)
  {
    SymbolT op;

    while ((op = succFunc()) != _endSymbol)
    {
      if (op == _orSymbol)
      {
        NFA<SymbolT>& rOperand = _popNFA();
        NFA<SymbolT>& lOperand = _popNFA();
        NFA<SymbolT>& result = _orInduction(lOperand, rOperand);
        _pushNFA(result); 
      }
      else
      {
        assert(false); // unreachable, that's a bug otherwise
      }
    }
  }

  void _treatCloseParenth()
  {
    _treatStackedOperators(
      [this]() {
        SymbolT op = _popOperator();
        return op == _openParenthSymbol ? _endSymbol : op;
      });
  }

  void _treatLambda()
  {
    NFA<SymbolT>& simpleNFA = _baseCase(_current());
    _pushNFA(simpleNFA);
  }

  SymbolT _current() const
  {
    return _workspace->expr[_workspace->index];
  }

  bool _ended() const
  {
    return _current() == _endSymbol;
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
    SymbolT current = _current();

    if (current == _starSymbol)
    {
      _treatStar();
    }
    else if (current == _orSymbol)
    {
      _treatOr();
    }
    else if (current == _openParenthSymbol)
    {
      _treatOpenParenth();
    }
    else if (current == _closeParenthSymbol)
    {
      _treatCloseParenth();
    }
    else
    {
      _treatLambda();
    }
  }

  void _unloadOperatorStack()
  {
    _treatStackedOperators(
      [this]() {
        if (_workspace->operatorStack.empty())
        {
          return _endSymbol;
        }
        else
        {
          return _popOperator();
        }
      });
  }

  void _finalNFAConcat()
  {
    while (!_workspace->nfaStack.empty())
    {
      auto& current = _popNFA();
      auto newInitial = _nfa.addState();
      _nfa.insert(current, newInitial, _nfa.getInitial());
      _nfa.replaceInitial(newInitial);
      delete &current;
    }
  }

  void _build()
  {
    for (_start(); !_ended(); _forward())
    {
      _shunt();
    }
    _unloadOperatorStack();
    _finalNFAConcat();
  }

public:
  NFABuilder(SymbolT const* expr) :
    _nfa(), _workspace(new Workspace(expr))
  {
    try
    {
      _build();
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
