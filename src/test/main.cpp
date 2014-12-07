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

#include <iostream>
#include <cassert>
#include <list>

#include "NFA.h"
#include "Regex.h"
#include "Lexer.h"
#include "NPIConvertor.h"
#include "Token.h"

// this function doesn't free data.
void testNFA()
{
  std::cout << "Testing NFA ..." << std::endl;

  NFA<char> nfa;

  // initial
  assert(nfa.getInitial() == 0);
  assert(nfa.isInitial(0));
  assert(nfa.getAcceptorSet().size() == 0);
  assert(nfa.isAcceptor(0) == false);
  assert(nfa.transitions(0, 'a').size() == 0);
  assert(nfa.epsilonTransitions(0).size() == 0);
  assert(nfa.epsilonClosure(0).size() == 1);
  assert(nfa.epsilonClosure(0).count(0) == 1);

  // add transition
  auto id = nfa.addState();
  assert(nfa.isInitial(id) == false);
  assert(nfa.isAcceptor(id) == false);

  // play with initial and acceptor
  nfa.replaceInitial(id);
  assert(nfa.isInitial(0) == false);
  assert(nfa.isInitial(id) == true);
  assert(nfa.getInitial() == id);
  nfa.replaceInitial(0);
  assert(nfa.getInitial() == 0);

  nfa.setAcceptor(id);
  assert(nfa.isAcceptor(id) == true);
  assert(nfa.getAcceptorSet().size() == 1);
  assert(nfa.getAcceptorSet().count(id) == 1);
  nfa.unsetAcceptor(id);
  assert(nfa.isAcceptor(id) == false);
  assert(nfa.getAcceptorSet().size() == 0);

  // add transistion
  nfa.addTransition(0, 'a', id);
  assert(nfa.transitions(0, 'a').size() == 1);
  assert(nfa.transitions(0, 'a').count(id) == 1);
  assert(nfa.transitions(0, 'b').size() == 0);
  nfa.addTransition(0, 'b', id);
  assert(nfa.transitions(0, 'b').size() == 1);
  assert(nfa.transitions(0, 'b').count(id) == 1);

  // add epsilonTransistion
  nfa.addEpsilonTransition(0, id);
  assert(nfa.epsilonTransitions(0).size() == 1);
  assert(nfa.epsilonTransitions(0).count(id) == 1);

  // epsilonClosure
  assert(nfa.epsilonClosure(0).size() == 2);
  assert(nfa.epsilonClosure(0).count(0) == 1);
  assert(nfa.epsilonClosure(0).count(id) == 1);

  auto id2 = nfa.addState();
  nfa.addEpsilonTransition(id, id2);

  assert(nfa.epsilonClosure(0).size() == 3);
  assert(nfa.epsilonClosure(0).count(0) == 1);
  assert(nfa.epsilonClosure(0).count(id) == 1);
  assert(nfa.epsilonClosure(0).count(id2) == 1);

  assert(nfa.epsilonClosure(1).size() == 2);
  assert(nfa.epsilonClosure(1).count(0) == 0);
  assert(nfa.epsilonClosure(1).count(id) == 1);
  assert(nfa.epsilonClosure(1).count(id2) == 1);

  nfa.addEpsilonTransition(id2, 0);

  assert(nfa.epsilonClosure(0).size() == 3);
  assert(nfa.epsilonClosure(0).count(0) == 1);
  assert(nfa.epsilonClosure(0).count(id) == 1);
  assert(nfa.epsilonClosure(0).count(id2) == 1);

  assert(nfa.epsilonClosure(1).size() == 3);
  assert(nfa.epsilonClosure(1).count(0) == 1);
  assert(nfa.epsilonClosure(1).count(id) == 1);
  assert(nfa.epsilonClosure(1).count(id2) == 1);

  nfa.addTransition(0, 'a', id2);
  assert(nfa.transitions(0, 'a').size() == 2);
  assert(nfa.transitions(0, 'a').count(id) == 1);
  assert(nfa.transitions(0, 'a').count(id2) == 1);

  nfa.clearAcceptorSet();
  assert(nfa.getAcceptorSet().size() == 0);

  // insert
  NFA<char> root;
  auto acceptorId = root.addState();
  root.setAcceptor(acceptorId);

  NFA<char> left(root);
  left.addTransition(0, 'l', acceptorId);

  NFA<char> right(root);
  right.addTransition(0, 'r', acceptorId);

  root.insert(left, 0, acceptorId);
  root.insert(right, 0, acceptorId);

  assert(root.epsilonClosure(0).size() == 3);
  auto const& set = root.epsilonTransitions(0);
  for (auto st : set)
  {
    assert(
        (root.transitions(st, 'l').size() == 0
      && root.transitions(st, 'r').size() == 1)
      ||(root.transitions(st, 'r').size() == 0
      && root.transitions(st, 'l').size() == 1)
    );
  }
}

typedef Token<char> TokenC;

static bool compareTokenCList(std::list<TokenC>const& l1, std::list<TokenC>const& l2)
{
  if (l1.size() != l2.size())
  {
    return false;
  }

  for (auto it1 = l1.begin(), it2 = l2.begin();
      it1 != l1.end(); it1++, it2++)
  {
    if (it1->getLabel() != it2->getLabel())
    {
      return false;
    }
  }
  return true;
}

static void showTokenList(std::list<TokenC> const& l)
{
  for (auto token : l)
  {
    std::cout << token.toString() << " ";
  }
  std::cout << std::endl;
}

void testLexer()
{
  std::cout << "Testing Lexer ..." << std::endl;

  Lexer<char> lexer1("to");
  std::list<TokenC> l1 {
    TokenC(TokenC::LAMBDA), TokenC(TokenC::CONCAT),
    TokenC(TokenC::LAMBDA), TokenC(TokenC::END)
  };
  assert(compareTokenCList(lexer1.collect(), l1));

  Lexer<char> lexer2("a(b|c)*d");
  std::list<TokenC> l2 {
    TokenC(TokenC::LAMBDA), TokenC(TokenC::CONCAT), TokenC(TokenC::LEFT_PARENTH),
    TokenC(TokenC::LAMBDA), TokenC(TokenC::OR), TokenC(TokenC::LAMBDA),
    TokenC(TokenC::RIGHT_PARENTH), TokenC(TokenC::STAR), TokenC(TokenC::CONCAT),
    TokenC(TokenC::LAMBDA), TokenC(TokenC::END)
  };
  assert(compareTokenCList(lexer2.collect(), l2));

}

void testNPIConvertor()
{

}

int main(int argc, char const *argv[])
{
  std::cout << "Let's test every classes one by one :" << std::endl;
  testNFA();
  testLexer();
  std::cout << "All the tests passed with success !!!" << std::endl;
  return 0;
}