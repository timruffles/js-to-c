# Todos

Next action: 
- look at how to break on exit - failing precondition at language.c:125
- why is `npm test -- --grep='global garbage'` (still) failing?
  - suspicious `[INFO] (gc.c:292:gcObjectFree) freeing string data 'arg0' at 0x100400408` - tho could be an call env from prev call?
  - still same bug, atomic might fix?:


[INFO] (/Users/timruffles/p/js-to-c/runtime/environments.c:32:envCreateForCall) Created call env 0x100806908 parent 0x100801a30, now looping over 1 args
[ERROR] (/Users/timruffles/p/js-to-c/runtime/language.c:125:jsValuePointer) Expected pointer value, got free space at 0x100801b40
  

## List

- interned strings shouldn't be subject to GC
- Adding stack trace library would be very useful!
- Why is Error undefined when thrown?
- Object System
  - debugging - list keys
  - Object.prototype -> prelude
  - prelude - it'd be really useful to define lots of the functionality in JS, and compile it (bootstrap the minimum required in C)
      - * immediate use case - implement `new Error` (see exceptions.toml)
  - method calls - setting this in `obj.method()`
  - real implementation of properties - descriptors etc
- Arrays
- varargs: currently stuff like console.log only takes one args
- functions passing too few arguments
  - should pass in undefineds
- warning on missing
- logical expressions
- Number formatting, e.g 1 -> 1, 0.1 -> 0.1, not 1.00000 and 0.10000
- var hoisting
  - variable
  - functions
- GC optimisation
  - at the mo free list fragments in a way that'd be trivial to defrag while processing (as lots of it is likely continugous)
  - currently will fail to allocate N bytes if there is plenty of space left, as long as all of the free chunks are < N big
- event system
    - promises (nice n simple, no need to do IO)
    - timers
    - input/output events

## Dones

- The array of pointers to function argument names needs to be heap allocated (or stored somewhere)
