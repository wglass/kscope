#Kscope#

My personal stab at the kaleidoscope LLVM tutorial found here:

http://llvm.org/docs/tutorial/index.html


## Deviations ##

As part of the learning experience this naturally evolved a bit
from the tutorial itself.  Here's a rundown of what's different:

### Language differences

#### Commas in prototypes
tutorial:
```
def foo(x y) x + y;
```
kscope:
```
def foo(x, y) x + y;
```
#### No user-defined operators
Might bring this stuff back now that the flex/bison combo is working
well enough.  It's really not a feature that serious languages should
have but then again this isn't a serious language.

### C++11 features

Part of the reason for all this is to (re)learn c++ and what's new in
c++11, so there are a handful of commits like f4b2108 that leverage
new features like

* [move semantics](http://www.codeproject.com/Articles/570638/Ten-Cplusplus11-Features-Every-Cplusplus-Developer#movesemantics)  for classes with pointer data
* [for-each style loops](http://www.codeproject.com/Articles/570638/Ten-Cplusplus11-Features-Every-Cplusplus-Developer#foreach) rather than plain-old indexed ones
* [unique_ptr](http://en.cppreference.com/w/cpp/memory/unique_ptr) "smart pointer" type for pointer member data
* there was a [strongly-typed enum](http://www.codeproject.com/Articles/570638/Ten-Cplusplus11-Features-Every-Cplusplus-Developer#stronglytypedenums) for the tokens at one point but
  using bison took care of that

Next up will be finding a way to use [lambdas](http://msdn.microsoft.com/en-us/library/dd293603.aspx) in a nice way, since
they're rad as hell.

### Style

CamelCase and pascalCase are used for class names but that's about it.
Evertyhing else adheres to an underscore style.  I guess this isn't
normally how c++ is done but I like it more so there.

## Building ##

###Requirements:
  * LLVM
  * bison >= 3.0
  * flex

###to build:
```
mkdir build && cd build

cmake ..

make
```
