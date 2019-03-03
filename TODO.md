# Todos

Next action: 
- need to fill in more of the language as it makes writing tests hard, TODOs in other tests aren't clear
  enough (note to self: read the skipped tests)

## List

- Object System
  - debugging - list keys
  - Object.prototype -> prelude
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
    - input/output events
    - ? promises (nice n simple, no need to do IO)

## Dones

- The array of pointers to function argument names needs to be heap allocated (or stored somewhere)
