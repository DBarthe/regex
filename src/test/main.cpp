
#include <iostream>
#include <cassert>

#include "NFA.h"

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
  NFA<char>::StateId id = nfa.addState();
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

  NFA<char>::StateId id2 = nfa.addState();
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
  NFA<char>::StateId acceptorId = root.addState();
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


int main(int argc, char const *argv[])
{
  std::cout << "Let's test every classes one by one :" << std::endl;
  testNFA();
  std::cout << "All the tests passed with success !!!" << std::endl;
  return 0;
}