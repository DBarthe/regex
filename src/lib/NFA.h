#ifndef NFA_H
#define NFA_H

#include <list>
#include <vector>
#include <map>
#include <initializer_list>

template <typename SymbolT>
class NFA {
public:
  // public types
  typedef unsigned int StateId;
  typedef std::list<StateId> StateSet;

  // ctor and dtor
  NFA() = default;
  ~NFA() = default;

  // getters and setters
  StateSet const& getAcceptorSet() const;
  void setAcceptor(StateId id, bool is=true);

  StateId getInitial() const;
  void replaceInitial(StateId id);

  // methods that add new states and transitions
  StateId addState();

  void addTransition(StateId srcId, SymbolT symbol, StateId dstId);
  void addEpsilonTransition(StateId srcId, StateId dstId);

  void insert(NFA<SymbolT> const& other, StateId in, StateId out);

  // methods that search and return state's subset
  StateSet const& epsilonClosure(StateId id) const;
  StateSet& epsilonClosure(std::initializer_list<StateId> stateSet) const;
  StateSet& epsilonClosure(StateSet const& stateSet) const;

  StateSet const& transition(StateId id, SymbolT symbol) const;
  StateSet const& epsilonTransition(StateId id) const;

private:
  StateId _initialState = 0;
  StateId _nextStateId = 1;
  
  StateSet _acceptorSet;

  typedef std::map<SymbolT, StateSet> StateTransitionMap;
  typedef std::vector<StateTransitionMap> TransitionTable; 

  TransitionTable _transitionTable;
};

#endif // NFA_H
