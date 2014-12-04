
#include <iostream>

#include "Regex.h"

int main(int argc, char const *argv[])
{
  std::cout << "And thanks for all the fish !" << std::endl;

  std::string test("toto");
  Regex re(test);


  return 0;
}