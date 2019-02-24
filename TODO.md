# Todos

Next action: 
- keep working on atomic problem - `./scripts/test-by-name 'garbage in loop'`
- weird getting:

[INFO] (objects.c:198:objectSet) Setting itemOne in object at 0x1006003b0
[INFO] (objects.c:204:objectSet) looking in 0x0 for props
[INFO] (objects.c:103:objectGet) Getting (
[INFO] (objects.c:103:objectGet) Getting ( 
- instanceof important - need to fill in more of the language
  as it makes writing tests hard, TODOs in other tests aren't clear
  enough (note to self: read the skipped tests)

## List

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
