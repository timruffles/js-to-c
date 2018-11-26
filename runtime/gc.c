#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include "lib/debug.h"
#include "gc.h"
#include "language.h"
#include "assert.h"
#include "objects.h"
#include "config.h"
#include "global.h"
#include "runtime.h"
#include "strings.h"
#include "functions.h"

#define ensureCallocBytes(V, M) V = calloc(1, M); assert(V != NULL);

typedef struct FreeSpace {
  GcHeader;
} FreeSpace;

typedef struct FreeNode {
    struct FreeNode* next;
    struct FreeNode* prev;
    struct FreeSpace* space;
} FreeNode;

static FreeNode* freeList;
static FreeNode* freeListTail;
static void* memory;
static void* memoryEnd;

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
 * To free item I
 *
 *   I->prev = I->next
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
    freeListTail = node;
}

static void freeListDelete(FreeNode* toRemove) {
   toRemove->prev = toRemove->next;
}

static void freeListAppend(FreeNode* toAdd) {
    freeListTail->next = toAdd;
    toAdd->prev = freeListTail;
    freeListTail = toAdd;
}

static void freeListFree(FreeNode* node) {
    free(node);
}

static void freeListFreeAll() {
    for(FreeNode* node = freeList;
        node;
        node = node->next
    ) {
        freeListFree(node);
    }
}

void gcInit() {
    uint64_t heapSize = configGet(HeapSizeConfig).uintValue;

    log_info("initialised gc with heap size %llu", heapSize);

    ensureCallocBytes(memory, heapSize);

    FreeSpace* space = memory;
    memoryEnd = (void*)((char*)memory + heapSize);

    *space = freeSpaceCreate(heapSize);

    FreeNode* node = freeNodeCreate(space);
    freeListInit(node);
}


void _gcTestInit() {
    if(memory != NULL) {
        freeListFreeAll();
        free(memory);
    }
    gcInit();
}


static FreeNode* findFreeSpace(size_t bytes) {
    FreeNode* found = NULL;
    for(FreeNode* candidate = freeList;
        candidate;
        candidate = candidate->next
    ) {
        if(candidate->space->size >= bytes) {
            found = candidate;
            break;
        }
    }
    return found;
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
static GcObject* allocateInNode(FreeNode* found, uint64_t bytes) {
    // this has already been checked to be positive
    uint64_t remaining = (uint64_t)(found->space->size - bytes);

    GcObject* allocatedAddress = (void*)found->space;
    bool splitRequired = remaining > sizeof(FreeSpace);
    // do we have remaining free space?
    uint64_t allocatedSize = splitRequired
        ? bytes
        : found->space->size;

    if(splitRequired) {
        FreeSpace* newSpace = (void*)(((char*)found->space) + bytes);
        *newSpace = (FreeSpace) {
            .type = FREE_SPACE_TYPE,
            .size = remaining,
        };
        found->space = newSpace;
    } else {
        freeListDelete(found);
    }

    *allocatedAddress = (GcObject) {
        .size = allocatedSize
    };

    return allocatedAddress;
}

void _gcRunGlobal(void) {
    RuntimeEnvironment* runtime = runtimeGet();
    _gcRun(runtime->gcRoots, runtime->gcRootsCount);
}

void* gcAllocate(size_t bytes, int type) {
    FreeNode* found = findFreeSpace(bytes);
    if(found == NULL) {
        log_info("Out of memory, GC running");
        _gcRunGlobal();

        found = findFreeSpace(bytes);
        // out of memory after gc run
        assert(found != NULL);
    }

    GcObject* allocated = allocateInNode(found, bytes);
    allocated->type = type;
    return allocated;
}

GcStats gcStats() {
    // TODO
    uint64_t heapSize = configGet(HeapSizeConfig).uintValue;
    return (GcStats) {
        .used = 0,
        .remaining = 0,
        .heapSize = heapSize,
    };
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
    log_info("traversed %p", object);
}

static void mark(GcObject* item) {
    if(!item->marked) {
        item->marked = true;
        traverse(item);
    }
}

static void gcObjectFree(GcObject* object) {
    log_info("Freeing %s at %p", gcObjectReflect(object).name, object);
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
        log_info("GC mark loop");
        mark((GcObject*)roots[i]);
    }

    runtimeGcTraverse((GcCallback*)mark);

    log_info("GC fully traversed %llu roots", rootCount);

    uint64_t freed = 0;

    // scan entire heap
    GcObject* toProcess;
    for(toProcess = memory;
        toProcess != memoryEnd && toProcess->type != UNITIALIZED_TYPE;
        toProcess = (void*)((char*)(toProcess) + toProcess->size)
        ) {
        if(toProcess->marked) {
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
    //RuntimeEnvironment* rt = runtimeGet();
    //rt->gcAtomicGroupIds[rt->gcAtomicGroupCount] = rt->gcAtomicGroupId;
    //rt->gcAtomicGroupId += 1;
    //rt->gcAtomicGroupCount += 1;
    //precondition(rt->gcAtomicGroupCount < RUNTIME_GC_ATOMIC_GROUP_MAX, "too many gc groups");
    return 4;
}

void gcAtomicGroupEnd(GcAtomicId* id) {
    //precondition(rt->gcAtomicGroupCount > 0, "exit before enter");
    //GcAtomicId current = rt->gcAtomicGroupIds[rt->gcAtomicGroupCount - 1];
    //precondition(current == id, "group %i did not exit before nested group %i", id);
    //rt->gcAtomicGroupCount -= 1;
}
