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

### Design

#### layout
Wherever possible, classes are in their own respective .cc files with
matching headers.  For example, there's a header-and-implementation
pair for each of the abstract syntax tree nodes under [src/ast](https://github.com/wglass/kscope/tree/master/src/ast/)

#### AST/Codegen decoupling

The IR codegen parts of the system have been entirely decoupled from
the AST.  Each node class has a codegen method like before, but an
IRRenderer class is passed in, and it's this class that provides all
of the llvm-specific context and helper methods for generating LLVM IR.

At some point in the future I'll probably write separate renderers for
other formats, like having the AST pretty-printed on the screen or something.

#### Bison/Flex for parsing/lexing

Instead of an ad-hoc parser that uses getchar() all over the place and
is tightly coupled to everything else there's now a separate parsing subsystem
with a lexer.ll file for token/lexer state and grammar.yy with a distilled,
purestrain Backus-Naur Form grammar.  The parser and lexer are abstracted
into an STree (syntax tree) class that can parse an input stream and set
its root node to the result of the parsing.

This simplifies the main.cpp REPL stuff immensely, std::cin is given to
the STree in chunks and the root node has it's IR rendered with an IRRenderer.
In the future it will probably be updated to abstract the REPL stuff somewhere
else and allow for file-reading depending on how ```kscope``` is invoked.


### Style

CamelCase and pascalCase are used for class names but that's about it.
Everything else adheres to an underscore style.  I guess this isn't
normally how c++ is done but underscore-style is superior so there.

## Building ##

###Requirements:
  * cmake
  * LLVM
  * bison >= 3.0
  * flex

NOTE: Installing llvm via homebrew didn't quite do it for me, doesn't seem to install the proper cmake
helper stuff so you gotta install from source.  It's a fairly painless procedure though:

http://llvm.org/docs/CMake.html

Source for 3.3 (the version I've been using) is here: http://llvm.org/releases/download.html#3.3
###to build:
```
mkdir build && cd build

cmake ..

make
```
