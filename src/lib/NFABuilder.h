#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include "NFA.h"

template <typename SymbolT>
class NFABuilder {
public:
  NFABuilder() = default;
  virtual ~NFABuilder() = default;

  virtual bool build(SymbolT const* expr) = 0;
  virtual NFA<SymbolT>& getResult() = 0;
  virtual NFA<SymbolT> const& getResult() const;
};

#endif // NFA_BUILDER_H
