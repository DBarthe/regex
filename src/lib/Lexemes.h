#ifndef LEXEMES_H
#define LEXEMES_H

template <typename SymbolT>
class Lexemes
{
public:
  static SymbolT const starSymbol;
  static SymbolT const orSymbol;
  static SymbolT const openParenthSymbol;
  static SymbolT const closeParenthSymbol;
  static SymbolT const endSymbol;

public:
  Lexemes() = delete;
  Lexemes(Lexemes const&) = delete;
  void operator=(Lexemes const&) = delete;

};

#endif // LEXEMES_H
