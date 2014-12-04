#ifndef REGEX_BASE_H
#define REGEX_BASE_H

#include "DFA.h"
#include "DFABuilder.h"
#include "DFASimulator.h"

template <typename SymbolT>
class RegexBase {
public:
  RegexBase(SymbolT const* expr);

  template <typename T&>
  RegexBase(T const& customExpr)
  : RegexBase(arrayOfCustom(customExpr)) {
  }

  ~RegexBase() = default;

  bool match(SymbolT const* input);

  template <typename T&>
  bool match(T const& customInput) {
    return match(arrayOfCustom(customInput));
  }

private:
  template <typename T&>
  static SymbolT const* arrayOfCustom(T const& custom);

  DFASimulator _simulator;
};

#endif // REGEX_BASE_H
