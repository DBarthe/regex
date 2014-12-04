#ifndef NFA_SIMULATOR_H
#define NFA_SIMULATOR_H

#include "NFA.h"

template <typename SymbolT>
class NFASimulator {
public:
  NFASimulator() = default;
  ~NFASimulator() = default;

  bool simulate(NFA<SymbolT> const& nfa, SymbolT const* input);

private:

};

#endif // NFA_SIMULATOR_H
