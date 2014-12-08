regex
=====

A small regex library written in C++11, basically for learning purpose.


## Compilation

First, ensure you have clang installed on your system.

Then:
```bash
$ make
```

That will generate:

 * __./libregex.so__ and __./libregex.a__: the library
 * __./match__: a command line program
 * __./test-regex__: the program that contains the unit tests 


## Example
```c++
#include "Regex.h"

Regex re("(a|b)*(c?|(def)+)");

if (re.match("abc"))
{
  // matched ...
}
```




