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

#ifndef NFA_SIMULATOR_H
#define NFA_SIMULATOR_H

#include <vector>
#include <stack>

#include "NFA.h"
#include "Lexemes.h"

template <typename SymbolT>
class NFASimulator
{
private:
  std::vector<bool> _alreadyIn;

  std::stack<StateId> _oldStates;
  std::stack<StateId> _newStates;

  NFA<SymbolT> const* _nfa = nullptr;


private:
  void _cleanUp()
  {
    _nfa = nullptr;
    _alreadyIn.clear();
    while (!_oldStates.empty())
    {
      _oldStates.pop();
    }
    while (!_newStates.empty())
    {
      _newStates.pop();
    }
  }

  void _init(NFA<SymbolT> const& nfa)
  {
    _nfa = &nfa;
    _alreadyIn.reserve(nfa.size());
    _alreadyIn.insert(_alreadyIn.end(), nfa.size(), false);
    StateSet& epsSet = nfa.epsilonClosure(nfa.getInitial());
    for (auto state : epsSet)
    {
      _oldStates.push(state);
    }
    delete &epsSet;
  }

  bool _isAlreadyIn(StateId state) const
  {
    return _alreadyIn[state];
  }

  void _setIn(StateId state)
  {
    _alreadyIn[state] = true;  
  }

  void _noMoreIn(StateId state)
  {
    _alreadyIn[state] = false;
  }

  void _addState(StateId state)
  {
    _newStates.push(state);
    _setIn(state);
    for (auto reachable : _nfa->epsilonTransitions(state))
    {
      if (!_isAlreadyIn(reachable))
      {
        _addState(reachable);
      }
    }
  }

  void _expand(SymbolT symbol)
  {
    while (!_oldStates.empty())
    {
      for (auto state : _nfa->transitions(_oldStates.top(), symbol))
      {
        if (!_isAlreadyIn(state))
        {
          _addState(state);
        }
      }
      _oldStates.pop();
    }

    while (!_newStates.empty())
    {
      StateId state = _newStates.top();
      _newStates.pop();
      _oldStates.push(state);
      _noMoreIn(state);
    }
  }

  bool _isAccepted()
  {
    while (!_oldStates.empty())
    {
      if (_nfa->isAcceptor(_oldStates.top()))
      {
        return true;
      }
      _oldStates.pop();
    }
    return false;
  }

public:
  NFASimulator() = default;
  ~NFASimulator() = default;

  bool simulate(NFA<SymbolT> const& nfa, SymbolT const* input)
  {
    _cleanUp();
    _init(nfa);
    for (size_t i = 0; input[i] != Lexemes<SymbolT>::endSymbol; i++)
    {
      SymbolT current = input[i];
      _expand(current);
    }
    return _isAccepted();
  }
};

#endif // NFA_SIMULATOR_H
