# Todos

Next action: 
- a compiler optimisation
  - string concat or addition both good examples
- CI!
- a demo site with side-by-side JS and C output
- need to fill in more of the language as it makes writing tests hard, TODOs in other tests aren't clear
  enough (note to self: read the skipped tests)

## List

- Number formatting, e.g 1 -> 1, 0.1 -> 0.1, not 1.00000 and 0.10000
- Arrays
- Object System
  - debugging - list keys
  - Object.prototype -> prelude
  - method calls - setting this in `obj.method()`
  - real implementation of properties - descriptors etc
- varargs: currently stuff like console.log only takes one args
- warning on missing
- var hoisting
  - variable
  - functions
- GC optimisation
  - at the mo free list fragments in a way that'd be trivial to defrag while processing (as lots of it is likely continugous)
  - currently will fail to allocate N bytes if there is plenty of space left, as long as all of the free chunks are < N big
- event system
    - input/output events
    - ? promises (nice n simple, no need to do IO)

## Minor

- `getTrue()`, `getNaN()` etc are wasteful and unnecessary, make constants. 

## Dones

- The array of pointers to function argument names needs to be heap allocated (or stored somewhere)
