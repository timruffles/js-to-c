# Todos

Next action: 
- why is `npm test -- --grep='global garbage'` (still) failing?
    - itCanReuseMemory unit test is failing, narrower failure will be easier to debug

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
