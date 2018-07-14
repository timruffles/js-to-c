# Notes

## 13 July 2018

Deciding on setjmp vs explicit handling has been going round back of my mind. Exceptions can be thrown by just about any code in JS, e.g the full horror of getters is clear here:

      b = 'yo';
      window.c = 'hi';
      
      var a = {
          get foo() {
              console.log("muahah");
              delete window.b;
              delete window.c;
          }
      }

      // throw - b is not defined
      console.log(a.foo, b, c);

I've roughly manually compiled the result of the two techniques below when compiling the following:

    try {
      console.log(a.foo, b());
    } catch(e) {
      console.log(typeof b);  
    }


### Setjmp

Primitives etc would use longjmp

    JsValue* objectGet(JsValue* val, ...) {
      if(isNullOrUndefined(val)) {
        // longjmp()
        throw(...)
      }
    }

So our compiled code would setup setjmp's at catches:

    userProgram() {
      // try, via setjmp - setjmp returns 0 when not returning from
      // a jmp
      if(!setjmp()) {
        // code for envGet(env, 'a')
        // code for objectGet(a, 'foo')
        // code for call to b()
        // code for envGet(env, 'console'), for the log
      } else {
        // < stack unwinding code >
        // create new env with 'e' bound to error
        // code for console.log etc
      }
    }

GC is interesting, for expressions that don't assign vars
but do create JSValues:

    f()()()()

what if third one threw? Where are the results being tracked?

We could associate them with a frame - e.g something like:

    JsValue* x = frameAllocate(frame);
    x = <any operation that can throw >

This way if the operation longjmps, we have the pointer referenced
in our frame and we can GC it when unwinding that frame.

OR - is that totally wrong-headed? Would this kind of intermediate
result be better implemented as automatic variables, which would
be cleaned up anyhow?

Simple example

    one();
  
    function one() {
      try {
        two();
      } catch(e) {

      }
    }

    function two() {
      var a = 1;
      three();
    }

    function three() {
      q();
    }

Compiled:

    void userProgram(Env* env) {
      envDeclareFunction(env, "one");
      // all hoisting

      <call one>
    }

    void one(Env* env) {
      JsValue* fval = envGet(env, one_interned);
      // we need to check if this is a valid call here
      ensureFunctionValue(fval);
      env2 = createCallEnvNoArguments(env);
      functionCall(fval, env2);
    }

    void two() {
      envDeclare(env, "a");
      envSet(env, "a", createNumber(1)) // this is heap alloc'd

      <call three>
    }

    void three() {
      <call q>
    }

    void envGet() {
      if(!defined) {
        JsValue* error = createError(...);
        activeError = error;
        throwingFrame = frame;
        longjmp();
      }
    }

    void ensureFunctionValue() {
    }

Seems like we're ok - all of the allocated objects (these will
always be JsValue) must be:

  1. assigned to a variable
  2. passed into a function (which ends up in an env)
  3. assigned to a data-structure (which must be in env)
  4. (later) passed to an event queue as a callback

So unwinding the stack frame should see everything free'd that needs
to be when GC occurs - all objects lacking path back to root
env will be collected.

### Explicit result

Alternatively, all the operations that could throw could return
a:

    struct ExecutionState {
      JsFrame* frame;
      // some enum
      ReturnType returnType;
      // which could be a JsError object
      JsValue* returnValue;
    }

So object get would be compiled to the following if we're in a catch

    ExecutionState* someUserFn(Env* env) {
        ExecutionState* s = objectGet(object, name);
        if(isExeception(s)) {
          goto catch1;
        }
      catch1:
        // ...
    }

or

    ExecutionState* someUserFn(Env* env) {
        ExecutionState* s = objectGet(object, name);
        if(isExeception(s)) {
          return s;
        }
        // ...
    }

## 12 July 2018

- it'd be nice to have one function for JSValue get
  that return an opaque 8 byte value, that can be
  cast back into appropriate value. However - seems better
  solution is to do the casting inside language.c via
  explicit getters - certainly abstracts the storage anyhow
- why isn't `const char* const` something we can assign to
  at compile time?
  - 
- why 'no previous extern declaration for non-static variable'
  - what's the harm of not making it explicit the var is
    just for use in a file/compilation unit - would be the
    default assumption?

- eh?
  my-bad-pointer-idea.c:44:8: error: expected parameter declarator
assert(sizeof JsValueContent == sizeof void *);
  - ah crap, I put it outside a function so it was interpreted
    as some type level stuff(?)
- given a variable of type T, and a variable of a type U, both
  n bytes, how do I do this:
    - T t1 = <valid value for T>
    - U u1 = <assign contents of t1 into u1, safely - so not casting as that could modify byte sequence>
    - T t2 = u1 <same process>
    - assertAllBytesSame(sizeof(t1), {&t1, &t2, &u1})

- if you're using stuff as bytes, use (void*)
  - otherwise casting may reverse or worse your args!
  
