This is a compiler for BMinor: a C-like educational language designed by Douglas Thain for [his compiler course](https://www3.nd.edu/~dthain/compilerbook/). My goal was to refresh C and learn a bit of X86_64 assembly. Writing was fun, but also a bit annoying: C is quite unforgiving. I still like it, but I'd use ML or even Python next time.

Some notes:
+ Not refactored, lots of ugly hacks. Coded, tested, abandoned.
+ No memory management. Leaking memory everywhere.
+ No optimisations. 
+ I might have extended the language's expressivity here and there, but I don't remember details.
+ The parser needs spaces around binary minus, so write 2 - 3, not 2-3.
+ The compiler emits NASM assembly.
+ Works surprisingly good!

Dependencies and building:
+ Linux: C/build-essential/bison/flex/nasm.
+ Run **make** to build.

How to use:
+ An example BMinor source *fibonnacci.bminor* is included from the [course's repository](https://github.com/dthain/compilerbook-examples). Let's torture it.
+ You can separately test the scanner and parser:
```console
./bminor -scan fibonnacci.bminor
./bminor -parse fibonnacci.bminor
```
+ You can also produce .dot files with slightly annotated BMinor program's AST:
```console
./bminor -dot fibonnacci.bminor > fibonacci.dot
dot -Tpdf fibonnacci.dot -o fibonnacci.pdf
```
+ To compile, assemble, link with a wrapper around some standard C functions, and run do:
```console
./bminor -compile fibonnacci.bminor > fibonacci.asm
make compile src=fibonacci
./fibonacci.bin
```
