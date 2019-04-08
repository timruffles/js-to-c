# C Notes

# 7 August 2018

- Standard header inclusion is super simple
- With pragma once you can have circular deps between header files
- Using forward declarations we can break circular deps

Using `clang -E` we can stop after pre-processor has run and just see the result:

    # 1 "../c-notes/type-dependencies/a.c"
    # 1 "<built-in>" 1
    # 1 "<built-in>" 3
    # 341 "<built-in>" 3
    # 1 "<command line>" 1
    # 1 "<built-in>" 2
    # 1 "../c-notes/type-dependencies/a.c" 2
    # 1 "../c-notes/type-dependencies/a.h" 1

    typedef struct A A;

    # 1 "../c-notes/type-dependencies/b.h" 1

    typedef struct {
        A* a;
        double dbl;
    } B;

    # 6 "../c-notes/type-dependencies/a.h" 2

    typedef struct A {
        B b;
    } A;

    # 2 "../c-notes/type-dependencies/a.c" 2

    int main() {
      A a = (A) {
          .b = (B) {
              .dbl = 6
          }
      };

      return a.b.dbl > 4;
    }

However, this can cause issues as anyone including the header files needs to know the order:

    #include "b.h"
    #include "a.h"

will break, as then we get:

    # 1 "../c-notes/type-dependencies/a.c"
    # 1 "../c-notes/type-dependencies/a.c" 2

    # 1 "../c-notes/type-dependencies/b.h" 1
    # 1 "../c-notes/type-dependencies/a.h" 1

    typedef struct A A;

    typedef struct A {
        B b;
    } A;
    # 4 "../c-notes/type-dependencies/b.h" 2

where B is clearly not yet declared.

