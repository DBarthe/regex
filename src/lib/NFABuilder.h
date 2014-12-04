#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include "NFA.h"

template <typename SymbolT>
class NFABuilder {
public:
  NFABuilder(SymbolT const* expr) = default;
  ~NFABuilder() = default;

  NFA<SymbolT>& collect();

private:
  NFA<SymbolT>* _nfa = nullptr;
};

#endif // NFA_BUILDER_H
