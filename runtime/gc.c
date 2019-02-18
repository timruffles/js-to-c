/**
 * Memory is a free list
 *
 *   free = LL for finding new area
 *
 * To allocate size B
 *
 *   candidate = free.head
 *   while candidate
 *     if capacity(candidate) <= B
 *       return split(candidate)
 *     else
 *       candidate = candidate.next
 *
 * Mark phase
 *
 *   forEach root in roots
 *     forEach item in traverse(root)
 *       item.marked = true
 *
 * Sweep phase
 *   
 *   i = 0;
 *   for item = memory[i]
 *     if item.marked
 *        item.marked = false
 *     else
 *        setFree(item)
 *        append item, free
 *     i += item.size
 */
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

#include "lib/debug.h"
#include "gc.h"
#include "language.h"
#include "assert.h"
#include "objects.h"
#include "global.h"
#include "runtime.h"
#include "strings.h"
#include "functions.h"
#include "_freelist.h"
#include "_memory.h"

// lives in JS heap to indicate its unused areas
typedef struct FreeSpace {
  GcHeader;
} FreeSpace;


// TODO might be worth moving these module variables to runtime
static void* memory;
static void* memoryEnd;
static FreeNode* freeList = NULL;

static const uint64_t NO_GROUP_ID = ULLONG_MAX;

static GcAtomicId gcCurrentGroupId() {
    RuntimeEnvironment* rt = runtimeGet();
    uint16_t depth = rt->gcAtomicGroupDepth;
    return depth == 0
        ? NO_GROUP_ID 
        : rt->gcAtomicGroupId + depth - 1;
}

static FreeSpace freeSpaceCreate(uint64_t size) {
    return (FreeSpace) {
        .type = FREE_SPACE_TYPE,
        .size = size,
    };
}

void gcInit(Config* config) {
    log_info("gcInit");
    uint64_t heapSize = config->heapSize;

    log_info("initialised gc with heap size %llu", heapSize);

    // we allocate a block of zeroed memory as our JS heap
    ensureCallocBytes(memory, heapSize);
    memoryEnd = (void*)((char*)memory + heapSize);

    // turn whole of heap into FreeSpace value
    FreeSpace* space = memory;
    *space = freeSpaceCreate(heapSize);

    // then initialise our free list with a single node pointing at our space
    freeListAppend(&freeList, space);

    log_info("gcInit returning, freeSpace at %p", space);
}


void _gcTestInit(Config* config) {
    if(memory != NULL) {
        freeListClear(&freeList);
        freeList = NULL;
        free(memory);
    }
    runtimeInit(config);
}

static FreeNode* findFreeSpace(size_t bytes) {
    FreeNode* found = NULL;
    FREE_LIST_ITERATE(&freeList, node) {
        FreeSpace* candidate = node->value;
        if(candidate->size >= bytes) {
            found = node;
            break;
        }
    }
    return found;
}


void _gcRunGlobal(void) {
    RuntimeEnvironment* runtime = runtimeGet();
    _gcRun(runtime->gcRoots, runtime->gcRootsCount);
}

// Allocate an object off the managed JS heap, so it's never scanned for freeing
static void* gcAllocateProtected(size_t bytes, int type) {
    GcObject* obj;
    ensureCallocBytes(obj, bytes);
    obj->type = type;
    return obj;
}

void* gcAllocate(size_t bytes, int type) {
    void* allocated = _gcAllocate(bytes, type);
    ensure(allocated != NULL, "Out of memory!");
    return allocated;
}

/**
 * Internal method, which doesn't exit on failure.
 *
 * Allocating in a free space node will either move the
 * header down, or allocate all of it and replace the node
 * (allocating more than required if remainder is < sizeof(FreeSpace)
 *
 *     0 FreeSpace header
 *     1 empty
 *     2 empty
 *     3 empty
 *
 *     0 AllocatedThing
 *     1 *cont*
 *     2 FreeSpace header
 *     3 empty
 *
 * or
 *
 *     0 AllocatedThing
 *     1 *cont*
 *     2 *cont*
 *     3 - over allocated, unused, included in size -
 */
void* _gcAllocate(size_t bytes, int type) {
    if(runtimeGet()->gcProtectAllocations) {
        log_info("Allocating protected");
        return gcAllocateProtected(bytes, type);
    }

    FreeNode* found = findFreeSpace(bytes);
    if(found == NULL) {
        log_info("Out of memory, GC running");
        _gcRunGlobal();

        found = findFreeSpace(bytes);
        // out of memory after gc run
        if(found == NULL) return NULL;
    }

    GcObject* allocated = (void*)found->value;

    uint64_t remaining = (uint64_t)(allocated->size - bytes);
    bool splitRequired = remaining > sizeof(FreeSpace);
    if(splitRequired) {
        // move the free node's space down past the used portion
        FreeSpace* newSpace = (void*)(((char*)found->value) + bytes);
        *newSpace = freeSpaceCreate(remaining);
        found->value = newSpace;

        *allocated = (GcObject) {
            .size = bytes
        };
    } else {
        freeListDelete(&freeList, found);
    }

    allocated->type = type;

    GcAtomicId id = gcCurrentGroupId();
    if(id != NO_GROUP_ID) {
        allocated->atomicGroupId = id;
    }

    return allocated;
}

static void mark(GcObject* item);

static void traverse(GcObject* object) {
    if(object->type == STRING_TYPE) {
        log_info("traverse string %p value '%s'", object, stringGetCString((void*)object));
    } else {
        log_info("traverse %s %p", gcObjectReflect(object).name, object);
    }

    switch(object->type) {
        case OBJECT_TYPE:
            objectGcTraverse((void*)object, (GcCallback*)mark);
            break;
        case STRING_TYPE:
            stringGcTraverse(object, (GcCallback*)mark);
            break;
        case FUNCTION_TYPE:
            functionGcTraverse(object, (GcCallback*)mark);
            break;
        // TODO - strings copy over string
        default:
            break;
    }
}

static void mark(GcObject* item) {
    if(!item->marked) {
        item->marked = true;
        traverse(item);
    }
}

static void gcObjectFree(GcObject* object) {
    if(object->type == STRING_VALUE_TYPE) {
        log_info("freeing string data '%s' at %p", _stringDebugValue((void*)object), object);
    } else if(object->type == STRING_TYPE) {
        log_info("freeing string '%s' at %p", stringGetCString((void*)object), object);
    } else {
        log_info("freeing %s at %p", gcObjectReflect(object).name, object);
    }
    // zero out the memory of the old object
    uint64_t size = object->size;
    memset(object, 0, object->size);

    // replace it with a free space header
    FreeSpace* newSpace = (void*)object;
    *newSpace = freeSpaceCreate(size);

    freeListAppend(&freeList, newSpace);
}


void _gcVisualiseHeap(GcVisualiseHeapOpts* opts) {
    static GcVisualiseHeapOpts defaultOpts = (GcVisualiseHeapOpts){
        .highlight = NULL
    };

    if(opts == NULL) {
      opts = &defaultOpts;    
    }

    bool foundHighlight = false;

    log_info("%p - Heap start", memory);
    GcObject* toProcess;
    for(toProcess = memory;
        (void*)toProcess < memoryEnd;
        toProcess = (void*)((char*)(toProcess) + toProcess->size)
        ) {
        if(toProcess->type == UNITIALIZED_TYPE) {
            log_info("%p - ERROR - Heap corruption - uninitialized memory", toProcess);
            break;
        } else {
            bool isHighlight = opts->highlight != NULL && opts->highlight == toProcess;
            log_info("%p - %12s %2i %llu%s", toProcess, gcObjectReflect(toProcess).name, toProcess->type, toProcess->size, isHighlight ? " FOUND" : "");
            if(isHighlight) {
                foundHighlight = true;
            }
        }
    }

    if(toProcess != memoryEnd) {
        log_info("%p - ERROR - Unexpected scan end", toProcess);
    }
    if(opts->highlight != NULL && !foundHighlight) {
        log_info("%p - ERROR - highlight pointer not found in heap", opts->highlight);
    }
    log_info("%p - Heap end", memoryEnd);
}

// roots: global environment
//        task queues pointing to ExecuableFunctions (which point to environments)
void _gcRun(JsValue** roots, uint64_t rootCount) {
    log_info("GC starting on %llu roots", rootCount);

    // For item in roots
    for(uint64_t i = 0;
        i < rootCount;
        i++) {
        log_info("marking root object at %p %i", roots[i], ((GcObject*)roots[i])->marked);
        mark((GcObject*)roots[i]);
    }

    runtimeGcTraverse((GcCallback*)mark);

    log_info("GC fully traversed %llu roots", rootCount);

    uint64_t freed = 0;

    const GcAtomicId groupId = gcCurrentGroupId();

    // scan entire heap
    GcObject* toProcess;
    for(toProcess = memory;
        toProcess != memoryEnd;
        toProcess = (void*)((char*)(toProcess) + toProcess->size)
        ) {

        log_info("scanned to %p", toProcess);

        if(toProcess->type == FREE_SPACE_TYPE) continue;

        if(toProcess->marked || toProcess->atomicGroupId >= groupId) {
            // reset for next GC run
            toProcess->marked = false;
        } else {
            gcObjectFree(toProcess);
            freed += toProcess->size;
        }
    }

    log_info("End of GC, memory end %p and got to %p", memoryEnd, toProcess);
    // we should have scanned whole heap, all memory is
    // either allocated or in a FreeSpace
    assert(toProcess == memoryEnd);

    log_info("GC complete, %lli bytes collected", freed);
}

GcAtomicId gcAtomicGroupStart() {
    // first group gets group ID, rest sequentially increase from there
    RuntimeEnvironment* rt = runtimeGet();
    uint64_t startDepth = rt->gcAtomicGroupDepth;
    rt->gcAtomicGroupDepth += 1;
    return rt->gcAtomicGroupId + startDepth;
}

void gcAtomicGroupEnd(GcAtomicId id) {
    RuntimeEnvironment* rt = runtimeGet();
    uint64_t nextToExit = rt->gcAtomicGroupId + rt->gcAtomicGroupDepth - 1;
    precondition(id == nextToExit, "atomic group exit out of order");
    rt->gcAtomicGroupDepth -= 1;
    if(rt->gcAtomicGroupDepth == 0) {
        // group complete, mark stale by incrementing counter
        rt->gcAtomicGroupId += 1;
    }
}

/**
 * Protected allocations live outside the managed JS heap. It's used
 * by the compiler for stuff like interned strings which it assumes will
 * never need to be GC'd
 **/
void gcStartProtectAllocations() {
    runtimeGet()->gcProtectAllocations = true;
    log_info("gcStartProtectAllocations");
}

void gcStopProtectAllocations() {
    runtimeGet()->gcProtectAllocations = false;
    log_info("gcStopProtectAllocations");
}
