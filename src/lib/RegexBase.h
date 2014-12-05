#ifndef REGEX_BASE_H
#define REGEX_BASE_H

#include "NFA.h"
#include "NFABuilder.h"
#include "NFASimulator.h"

template <typename SymbolT>
class RegexBase
{
public:
  RegexBase(SymbolT const* expr) :
    _builder(expr)
  {}

  template <typename T>
  RegexBase(T const& customExpr) :
    RegexBase(arrayOfCustom(customExpr))
  {}

  ~RegexBase() = default;

  bool match(SymbolT const* input) const
  {
    NFASimulator<SymbolT> simulator;
    return simulator.simulate(_builder.collect(), input);
  }

  template <typename T>
  bool match(T const& customInput) const {
    return match(arrayOfCustom(customInput));
  }

private:
  template <typename T>
  static SymbolT const* arrayOfCustom(T const& custom) { return nullptr; }

  NFABuilder<SymbolT> _builder;
};

#endif // REGEX_BASE_H
