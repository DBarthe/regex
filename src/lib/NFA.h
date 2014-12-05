#ifndef NFA_H
#define NFA_H

#include <cassert>

#include <set>
#include <vector>
#include <map>
#include <stack>
#include <utility>
#include <initializer_list>
#include <algorithm>

template <typename SymbolT>
class NFA {
public:
  // public types
  typedef unsigned int StateId;
  typedef std::set<StateId> StateSet;

private:
  // private types
  // sub-types
  typedef std::map<SymbolT, StateSet> _SymbolTransMap;
  typedef StateSet _EspilonTransSet;
  typedef std::pair<_EspilonTransSet, _SymbolTransMap> _TransSetPair;

  // main type (vector of pair <set, map>)
  typedef std::vector<_TransSetPair> _TransitionTable; 

  // constants
  static const size_t _CHUNK_SIZE = 64;

  // attributes
  StateId _initialState;
  StateId _nextState;
  StateSet _acceptorSet;
  _TransitionTable _transTable;

  StateSet const _emptyConstSet;

private:
  // privates methods
  void _maybeIncreaseCapacity()
  {
    if (_transTable.size() == _transTable.capacity())
    {
      _increaseCapacity();
    }
  }

  void _increaseCapacity() {
    size_t newCapacity = _transTable.capacity() + _CHUNK_SIZE;
    _transTable.reserve(newCapacity);
  }

  bool _exists(StateId id) const
  {
    return id < _nextState;
  }

  static bool _isElement(StateSet const& set, StateId id)
  {
    return set.find(id) != set.end();
  }

public:
  // ctor and dtor

  NFA() :
    _initialState(0),
    _nextState(0),
    _acceptorSet(),
    _transTable(),
    _emptyConstSet()
  {
    StateId id = addState();
    // ensure the first added state is the initial, at start.
    assert (id == _initialState); 
  }

  ~NFA()
  {
    // nothing to delete since all containers are filled with 'emplace'
  }

  // Getters, setters, isers.
  
  StateSet const& getAcceptorSet() const
  {
    return _acceptorSet;
  }

  void setAcceptor(StateId id, bool value)
  {
    if (value)
    {
      setAcceptor(id);
    }
    else
    {
      unsetAcceptor(id);
    }
  }

  void setAcceptor(StateId id)
  {
    if (_exists(id) && !isAcceptor(id))
    {
      _acceptorSet.insert(id);
    }
  }

  void unsetAcceptor(StateId id) 
  {
    if (_exists(id))
    {
      _acceptorSet.erase(id);
    }
  }

  bool isAcceptor(StateId id) const
  {
    return _exists(id) && _isElement(_acceptorSet, id);
  }

  StateId getInitial() const
  {
    return _initialState;
  }

  void replaceInitial(StateId id)
  {
    assert(_exists(id)); // this means a bug

    if (_exists(id))
    {
      _initialState = id;
    }
  }

  bool isInitial(StateId id) const
  {
    return _initialState == id;
  }

  // methods that add new states and transitions

  StateId addState()
  {
    StateId state = _nextState;
    _nextState += 1;

    // create a new empty case indexed by the new state ...
    // ... in the transition table
    _maybeIncreaseCapacity();
    _transTable.emplace_back(); 
    assert(_transTable.size() == _nextState);

    return state;
  }

  void addTransition(StateId src, SymbolT symbol, StateId dst)
  {
    assert(_exists(src)), assert(_exists(dst));

    if (_exists(src) && _exists(dst))
    {
      (void) _transTable[src].second[symbol].emplace(dst);
    }
  }

  void addEpsilonTransition(StateId src, StateId dst)
  {
    assert(_exists(src)), assert(_exists(dst));

    if (_exists(src) && _exists(dst))
    {
      (void) _transTable[src].first.emplace(dst);
    }  
  }

  void insert(NFA const& other, StateId in, StateId out)
  {
    assert(_exists(in)), assert(_exists(out));

    if (_exists(in) && _exists(out))
    {
      // clone transition table
      _TransitionTable clone(other._transTable);

      // update each state id
      // for each state
      for (auto& pair : clone)
      {
        // in epsilon transitions
        for (auto& id : pair.first)
        {
          id += _nextState;
        }
        // in symbol transitions
        for (auto& innerPair : pair.first)
        {
          innerPair.id += _nextState;
        }
      }

      // append the other's cloned and updated transition table.
      _transTable.insert(_transTable.end(), clone.begin(), clone.end());

      // save then update this _nextState
      StateId savedNextState = _nextState;
      _nextState += other._nextState;

      // make the links between the old and the new states.
      // in -> other's initial
      assert(_exists(other.getInitial() + savedNextState));
      addEpsilonTransition(in, other.getInitial() + savedNextState);
      // other's acceptors -> out
      for (auto id : other.getAcceptorSet())
      {
        assert(_exists(id + savedNextState));
        addEpsilonTransition(id + savedNextState, out);
      }
    }
  }

  // methods that search and return state's subset
  StateSet& epsilonClosure(StateId id) const
  {
    return epsilonClosure( { id } );
  }

  // algorithm present
  StateSet& epsilonClosure(std::initializer_list<StateId> stateList) const
  {
    StateSet set(stateList); // that removes doublons
    return epsilonClosure(set);
  }

  // This algorithm is pulled from the Dragon Book (Fig 3.33).
  StateSet& epsilonClosure(StateSet const& set) const
  {
    auto& resultSet = *new StateSet(set); 
    std::stack<StateId> stack;

    // fill the stack
    for (auto state : set)
    {
      stack.push(state);
    }

    while (!stack.empty())
    {
      StateId current = stack.top();
      stack.pop();
      // for each state reachable from current by an epsilon transition 
      for (auto reachable : _transTable[current].first)
      {
        if (!_isElement(resultSet, reachable))
        {
          resultSet.insert(reachable);
          stack.push(reachable);
        }        
      }
    }

    return resultSet;
  }

  StateSet const& transitions(StateId id, SymbolT symbol) const
  {
    assert (_exists(id));

    if (!_exists(id))
    {
      return _emptyConstSet;
    }
    else
    {
      auto const& it = _transTable[id].second.find(symbol);
      if (it == _transTable[id].second.end())
      {
        return _emptyConstSet;
      }
      else
      {
        return it->second;
      }
    }
  }

  StateSet const& epsilonTransitions(StateId id) const
  {
    assert(_exists(id));

    if (!_exists(id))
    {
      return _emptyConstSet;
    }
    else
    {
      return _transTable[id].second;
    }
  }
};

#endif // NFA_H
