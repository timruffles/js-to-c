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

#define ensureCallocBytes(V, M) V = calloc(1, M); assert(V != NULL);

// lives in JS heap to indicate its unused areas
typedef struct FreeSpace {
  GcHeader;
} FreeSpace;

// space tracking, which lives outside JS heap
typedef struct FreeNode {
    struct FreeNode* next;
    struct FreeNode* prev;
    struct FreeSpace* space;
} FreeNode;

// TODO might be worth moving these module variables to runtime
static FreeNode* freeList;
static void* memory;
static void* memoryEnd;

static const uint64_t NO_GROUP_ID = ULLONG_MAX;

static GcAtomicId gcCurrentGroupId() {
    RuntimeEnvironment* rt = runtimeGet();
    uint16_t depth = rt->gcAtomicGroupDepth;
    return depth == 0
        ? NO_GROUP_ID 
        : rt->gcAtomicGroupId + depth - 1;
}

static FreeNode* freeNodeCreate(FreeSpace* space) {
    FreeNode* node;
    ensureCallocBytes(node, sizeof(FreeNode));
    *node = (FreeNode) {
        .space = space,
    };
    return node;
}

static FreeSpace freeSpaceCreate(uint64_t size) {
    return (FreeSpace) {
        .type = FREE_SPACE_TYPE,
        .size = size,
    };
}

static void freeListInit(FreeNode* node) {
    freeList = node;
}

static void freeListDelete(FreeNode* toRemove) {
    // not first item
    if(toRemove == freeList) {
        if(toRemove->next) {
            freeList = toRemove->next;
        }
    } else {
       toRemove->prev->next = toRemove->next;
       free(toRemove);
    }
}

static void freeListAppend(FreeNode* toAdd) {
    FreeNode* currentHead = freeList;
    toAdd->next = currentHead;
    currentHead->prev = toAdd;
    freeList = toAdd;
}

static void freeListFreeAll() {
    for(FreeNode* node = freeList;
        node;
        node = node->next
    ) {
        free(node);
    }
}

void gcInit(Config* config) {
    uint64_t heapSize = config->heapSize;

    log_info("initialised gc with heap size %llu", heapSize);

    ensureCallocBytes(memory, heapSize);

    FreeSpace* space = memory;
    memoryEnd = (void*)((char*)memory + heapSize);

    *space = freeSpaceCreate(heapSize);

    FreeNode* node = freeNodeCreate(space);
    freeListInit(node);
}


void _gcTestInit(Config* config) {
    if(memory != NULL) {
        freeListFreeAll();
        free(memory);
    }
    runtimeInit(config);
}


static FreeNode* findFreeSpace(size_t bytes) {
    FreeNode* found = NULL;
    for(FreeNode* candidate = freeList;
        candidate != NULL;
        candidate = candidate->next
    ) {
        if(candidate->space->size >= bytes) {
            found = candidate;
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


/**
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
void* gcAllocate(size_t bytes, int type) {
    if(runtimeGet()->gcProtectAllocations) {
        return gcAllocateProtected(bytes, type);
    }

    FreeNode* found = findFreeSpace(bytes);
    if(found == NULL) {
        log_info("Out of memory, GC running");
        _gcRunGlobal();

        found = findFreeSpace(bytes);
        // out of memory after gc run
        assert(found != NULL);
    }

    GcObject* allocated = (void*)found->space;

    uint64_t remaining = (uint64_t)(allocated->size - bytes);
    bool splitRequired = remaining > sizeof(FreeSpace);
    if(splitRequired) {
        // move the free node's space down past the used portion
        FreeSpace* newSpace = (void*)(((char*)found->space) + bytes);
        *newSpace = freeSpaceCreate(remaining);
        found->space = newSpace;

        *allocated = (GcObject) {
            .size = bytes
        };
    } else {
        freeListDelete(found);
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
    uint64_t size = object->size;
    memset(object, 0, object->size);
    FreeSpace* newSpace = (void*)object;
    *newSpace = freeSpaceCreate(size);
    freeListAppend(freeNodeCreate(newSpace));
}

void _gcVisualiseHeap() {
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
            log_info("%p - %12s %2i %llu", toProcess, gcObjectReflect(toProcess).name, toProcess->type, toProcess->size);
        }
    }
    if(toProcess != memoryEnd) {
        log_info("%p - ERROR - Unexpected scan end", toProcess);
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
}

void gcStopProtectAllocations() {
    runtimeGet()->gcProtectAllocations = false;
}
