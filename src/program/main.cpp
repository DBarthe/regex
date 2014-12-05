
#include <iostream>

#include "Regex.h"

int main(int argc, char const *argv[])
{
  std::cout << "And thanks for all the fish !" << std::endl;

  if (argc >= 3)
  {
    Regex re(argv[1]);
    if (re.match(argv[2]))
    {
      std::cout << "matched" << std::endl;
    }
    else
    {
      std::cout << "mismatched" << std::endl;
    }
  }

  return 0;
}