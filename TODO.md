# Todos

Next action: why is `npm test -- --grep='global garbage'` failing?

- console.log fn env is NULL
- not freeing anything before bug runs
- ahh - are we freeing the call env!
- okay, nasty thing
  - many operations in the compiler that are atomic
    from the POV of GC (creating a call env) can trigger GC,
    so we'll need to mark this as a 'in progress' operation, and
    keep these objects in a GC buffer to avoid freeing half of them


```
int id = gcAtomicAllocationGroupEnter()
...
gcAtomicAllocationGroupExit(id)
```

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
- GC optimisation - at the mo free list fragments in a way that'd be trivial to defrag while processing (as lots of it is likely continugous)

## Dones

- The array of pointers to function argument names needs to be heap allocated (or stored somewhere)
