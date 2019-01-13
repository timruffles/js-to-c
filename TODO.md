# Todos

Next action: why is `npm test -- --grep='global garbage'` (still) failing? Recent to old log:

- adding stack trace library would be very useful!
- got reuse working, but now having an issue with objects either
  being GC'd incorrectly or moved? This assertion is failing in two tests

```
assert(JS_GET(root, "liveOne") == liveOne);
```

- seems like the free list isn't being pushed down, not reusing space
  - debuggin this is a pain... more tools?
- free list code is clearly rather borked...
- rewrote config system for simplicity, and wrote failing test
[INFO] (gc.c:232:gcAllocate) Allocated string at 0x101f00320
[ERROR] (language.c:202:jsValueReflect) Non JSValue free space
- mm, config system is kinda terrible
  - look online for a better one, would be nice to be
    able to init a subsystem for testing
- ah, looks like allocation in free zones is totally borked - all allocations hitting same area:

[INFO] (runtime/gc.c:233:gcAllocate) Allocated stringValue at 0x7fcbdf8037e0
[INFO] (runtime/gc.c:233:gcAllocate) Allocated string at 0x7fcbdf8037e0
[INFO] (runtime/objects.c:196:objectSet) Setting   in object at 0x7fcbdf8037a0
[INFO] (runtime/objects.c:202:objectSet) looking in 0x7fcbdf801678 for props
[INFO] (runtime/gc.c:233:gcAllocate) Allocated stringValue at 0x7fcbdf8037e0
[INFO] (runtime/gc.c:233:gcAllocate) Allocated string at 0x7fcbdf8037e0
[INFO] (runtime/environments.c:51:envGet) Looked up   in 0x7fcbdf8037a0 got type object
[INFO] (runtime/environments.c:51:envGet) Looked up i in 0x7fcbdf801028 got type number
[INFO] (runtime/gc.c:233:gcAllocate) Allocated number at 0x7fcbdf8037e0
[INFO] (runtime/gc.c:233:gcAllocate) Allocated objectValue at 0x7fcbdf801678
[INFO] (runtime/gc.c:233:gcAllocate) Allocated object at 0x7fcbdf8037e0
[INFO] (runtime/environments.c:51:envGet) Looked up i in 0x7fcbdf801028 got type number
[INFO] (runtime/objects.c:196:objectSet) Setting hiThere in object at 0x7fcbdf8037e0
[INFO] (runtime/objects.c:202:objectSet) looking in 0x7fcbdf8037e0 for props
[ERROR] (runtime/language.c:120:jsValuePointer) Expected pointer value, got objectValue at 0x7fcbdf801678
  
- odd, we have `objectVal->object->properties == objectVal!`
- mm, now it seems like we're allocating over objs still referenced...
  - 
  - `[ERROR] (/Users/timruffles/dev/p/js-to-c/runtime/language.c:120:jsValuePointer) Expected pointer value, got objectValue at 0x7fd243001678`
- added working atomic operator and passing `itCanPreventGcInTheMiddleOfAGroupOfOperations`, now need to wrap some ops with it
- okay, nasty thing
  - many operations in the compiler that are atomic
    from the POV of GC (creating a call env) can trigger GC,
    so we'll need to mark this as a 'in progress' operation, and
    keep these objects in a GC buffer to avoid freeing half of them
- ahh - are we freeing the call env!
- not freeing anything before bug runs
- console.log fn env is NULL


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
- GC optimisation
  - at the mo free list fragments in a way that'd be trivial to defrag while processing (as lots of it is likely continugous)
  - currently will fail to allocate N bytes if there is plenty of space left, as long as all of the free chunks are < N big
- event system
    - promises (nice n simple, no need to do IO)
    - timers
    - input/output events

## Dones

- The array of pointers to function argument names needs to be heap allocated (or stored somewhere)
