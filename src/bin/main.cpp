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