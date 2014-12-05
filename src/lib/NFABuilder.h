#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include <stack>
#include <stdexcept>

#include "NFA.h"
#include "Lexemes.h"

template <typename SymbolT>
class NFABuilder
{
private:

  enum TokenLabel
  {
    TOK_STAR,
    TOK_OR,
    TOK_OPARENTH,
    TOK_CPARENTH,
    TOK_CONCAT,
    TOK_END,
    TOK_LAMBDA
  };

  struct Token
  {
    Token(TokenLabel _label, SymbolT _value=Lexemes<SymbolT>::endSymbol) :
      label(_label), value(_value)
    {}

    TokenLabel label;
    SymbolT value;
  };

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

  void _maybePushConcat()
  {
    switch (_next().label)
    {
      case TOK_LAMBDA:
      case TOK_OPARENTH:
        _pushOperator(Token(TOK_CONCAT));  
      default:;
    }
  }

  void _treatStar()
  {
    NFA<SymbolT>& operand = _popNFA();
    NFA<SymbolT>& result = _starInduction(operand);
    _pushNFA(result);
    _maybePushConcat();
  }

  void _treatOr()
  {
    _pushOperator(Token(TOK_OR));
  }

  void _treatOpenParenth()
  {
    _pushOperator(Token(TOK_OPARENTH));
  }

  void _treatStackedOperators(
    std::function<TokenLabel ()> succFunc)
  {
    TokenLabel label ;

    while ((label = succFunc()) != TOK_END)
    {
      switch (label)
      {
        case TOK_OR:
        {
          NFA<SymbolT>& rOperand = _popNFA();
          NFA<SymbolT>& lOperand = _popNFA();
          NFA<SymbolT>& result = _orInduction(lOperand, rOperand);
          _pushNFA(result);
        }
        break;
  
        case TOK_CONCAT:
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
        return op.label == TOK_OPARENTH
          ? TOK_END
          : op.label;
      });
    _maybePushConcat();
  }

  void _treatLambda()
  {
    NFA<SymbolT>& simpleNFA = _baseCase(_current().value);
    _pushNFA(simpleNFA);
    _maybePushConcat();
  }

  Token _current() const
  {
    return _workspace->tokens[_workspace->index];
  }

  Token _next() const
  {
    return _ended()
      ? _current()
      :  _workspace->tokens[_workspace->index+1];
  }

  bool _ended() const
  {
    return _current().label == TOK_END;
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
    switch (_current().label)
    {
      case TOK_STAR:      _treatStar();         break;
      case TOK_OR:        _treatOr();           break;
      case TOK_OPARENTH:  _treatOpenParenth();  break;
      case TOK_CPARENTH:  _treatCloseParenth(); break;
      case TOK_LAMBDA:    _treatLambda();       break;
      default:;
    }
  }

  void _unloadOperatorStack()
  {
    _treatStackedOperators(
      [this]() {
        if (_workspace->operatorStack.empty())
        {
          return TOK_END;
        }
        else
        {
          return _popOperator().label;
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

  static size_t _exprSize(SymbolT const* expr)
  {
    size_t len = 0;
    while (expr[len] != Lexemes<SymbolT>::endSymbol)
    {
      len++;
    }
    return len;
  }

  void _tokenize(SymbolT const* expr)
  {
    _workspace->tokens.reserve(_exprSize(expr) + 1);

    for (size_t i = 0; expr[i] != Lexemes<SymbolT>::endSymbol; i++)
    {
      SymbolT current = expr[i];
      TokenLabel label;
      if (current == Lexemes<SymbolT>::starSymbol)
      {
        label = TOK_STAR;
      }
      else if (current == Lexemes<SymbolT>::orSymbol)
      {
        label = TOK_OR;
      }
      else if (current == Lexemes<SymbolT>::openParenthSymbol)
      {
        label = TOK_OPARENTH;
      }
      else if (current == Lexemes<SymbolT>::closeParenthSymbol)
      {
        label = TOK_CPARENTH;
      }
      else
      {
        label = TOK_LAMBDA;
      }
      _workspace->tokens.emplace_back(label, current);
    }
    _workspace->tokens.emplace_back(TOK_END);
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
