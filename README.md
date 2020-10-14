# JS to C 

A compiled implementation of [ES5](https://www.ecma-international.org/ecma-262/5.1/#sec-11.6.1), targetting C. I've finished the project as it now supports the parts of the language I found most interesting for a compiled implementation:

- ✅ closures
- ✅ garbage collection
- ✅ exceptions
- ✅ event based IO
- ✅ object system - prototypes etc 

Read my [notes](NOTES.md) for a rough dev notes, or read more polished write ups on [my blog](https://timr.co).

Next thing to do - see [TODO.md](TODO.md).

Example
-------

The following JS

```javascript
function fact(n) {
    return n < 3 ? n : n * fact(n - 1);
}
console.log(fact(5));
```

outputs as a C program. For instance the compiled `fact` function looks like:

```c
// ...
JsValue *fact_1(Env *env) {
  JsValue *return_2;
  JsValue *left_5 = (envGet(env, interned_7) /* n */);
  JsValue *right_6 = (jsValueCreateNumber(3));
  JsValue *conditionalPredicate_4 = (LTOperator(left_5, right_6));
  JsValue *conditionalValue_3;
  if (isTruthy(conditionalPredicate_4)) {
    return_2 = (envGet(env, interned_7) /* n */);
  } else {
    JsValue *left_8 = (envGet(env, interned_7) /* n */);
    JsValue *callee_10 = (envGet(env, interned_11) /* fact */);
    JsValue *left_12 = (envGet(env, interned_7) /* n */);
    JsValue *right_13 = (jsValueCreateNumber(1));
    JsValue *call10Arg_0 = (subtractOperator(left_12, right_13));
    JsValue *args_10[] = {call10Arg_0};
    JsValue *right_9 = (functionRunWithArguments(callee_10, env, args_10, 1));
    return_2 = (multiplyOperator(left_8, right_9));
  }
  return return_2;
}
// ...
```

Guide
-------

    ./runtime
        the C implementation of the language runtime
    ./src
        the TypeScript compiler

## Testing

### Functional

Run `npm test` to run/update tests. `./scripts/test-by-name $name` to run a single test.

Failed tests provide a command to compile and run the C program with the debugger.

### Unit

The runtime has unit tests - use the `make test` in `./runtime` to run them, and `./runtime/scripts/compile-test-loop test-file-basename`.

## Runtime

Runtime C libraries for js-to-c.

### Installing

Dependencies:

- clang
- Make
- CMake (for libuv)

I'm trying to limit my focus to learning about compilation, so I've made no attempt to make this cross platform. It shouldn't be too tricky, libuv is the big dependency and it's cross platform.

    make install

### Developing

Builds and runs all tests:

    make test

### Libraries

See `lib` - currently just

- debug.h 
  - debug macros

## Dynamic libraries

The runtime is built into a dynamic library to avoid recompiling. There's also a prelude, which allows langauge features to be implemented in JS that's pre-compiled to C. 

## Runtime

Files prefixed `_` are private to the runtime and should not be relied on.

## Potential issues

`grep` for `HMM` to see potential problems I'm leaving alone for now as they seem tricky to solve properly/I'm unsure they'll be a problem in practice. Good place to look if weird bugs occur.

