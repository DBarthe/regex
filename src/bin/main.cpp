
#include <iostream>

#include "Regex.h"

int main(int argc, char const *argv[])
{
  if (argc < 3)
  {
    std::cerr << "usage: " << argv[0] << "regexp string" << std::endl;
    return 1;
  }
  else
  {
    try
    {
      Regex re(argv[1]);
      if (re.match(argv[2]))
      {
        std::cout << "matched" << std::endl;
        return 0;
      }
      else
      {
        std::cout << "mismatched" << std::endl;
        return 1;
      }
    }
    catch (std::invalid_argument e)
    {
      std::cerr << "syntax error" << std::endl;;
      return 1;
    }
  }
  return 0;
}