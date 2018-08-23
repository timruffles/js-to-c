#include <stdint.h>
#include <stddef.h>

#include "lib/debug.h"
#include "gc.h"
#include "language.h"
#include "assert.h"
#include "objects.h"
#include "config.h"
#include "global.h"
#include "runtime.h"

#define ensureCalloc(V, M) V = calloc(1, M); assert(V != NULL);

typedef struct FreeNode {
    struct FreeNode* next;
    struct FreeNode* prev;
    struct char* memory;
} FreeNode;

static FreeNode* freeList;
static FreeNode* freeListTail;
static char* memory;


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

void freeListDelete(FreeNode* toRemove) {
   toRemove->prev = toRemove->next;
}

void freeListAppend(FreeNode* toAdd) {
    freeListTail->next = toAdd;
    freeListTail = toAdd;
}

void freeListFreeAll() {
}

void gcInit() {
    ConfigValue heapSize = configGet(HeapSizeConfig);

    log_info("initialised gc with heap size %llu", heapSize.uintValue);

    ensureCalloc(memory, heapSize);

    FreeSpace* space = memory;
    *space = (FreeSpace) {
        .type = FREE_SPACE_TYPE,
        .size = sizeof(FreeSpace),
        .space = memory,
        .capacity = heapSize,
    };

    FreeNode node* = ensureCalloc(sizeof(FreeNode));
    *node = (FreeNode) {
        .space = node,
    };
    freeList = node;
}


void _gcTestInit() {
    if(memory != NULL) {
        freeListFreeAll(freeSpace);
        free(memory);
    }
    gcInit();
}

void* gcAllocate2(size_t bytes, int type) {
    GcObject* allocated = gcAllocate(bytes);
    allocated->type = type;
    return allocated;
}

FreeNode* findFreeSpace() {
    FreeNode found*;
    for(FreeNode candidate* = freeList;
        candidate;
        candidate = candidate->space->capacity >= bytes
    ) {
        return candidate;
    }
}

void* allocateInNode(FreeNode* found, uint64_t bytes) {
    // this has already been checked to be positive
    uint64_t remaining = (uint64_t)(found->space->capacity - bytes);
    void* allocatedSpace = found->space->memory;
    found->space->memory += bytes;
    found->space->capacity -= bytes;
    if(remaining == 0) {
        freeListDelete(found);
    }
    return allocatedSpace;
}

void* gcAllocate(size_t bytes) {
    FreeNode* found = findFreeSpace();
    if(found == NULL) {
        log_info("Out of memory, GC running");
        RuntimeEnvironment* runtime = runtimeGet();
        _gcRun(runtime->gcRoots, runtime->gcRootsCount);

        found = findFreeSpace();
        // out  of memory
        assert(found != NULL);
    }

    return allocateInNode(found, bytes);
}

GcStats gcStats() {
    return (GcStats) {
        .used = 0,
        .remaining = 0,
    };
}

static uint64_t gcObjectSize(GcObject* object) {
    assert(object->next != NULL);
    return (uint64_t)object->next - (uint64_t)object;
}

static GcObject* mark(GcObject* item) {
    item->marked = true;
    traverse(item);
}

static void traverse(GcObject* object) {
    switch(object->type) {
        case OBJECT_TYPE:
            objectGcTraverse((void*)object, (GcCallback*)mark);
            break;
        // TODO - strings copy over string
        default:
            break;
    }
}

/**
 * GC uses something like the Cheny algorithm.
 *
 * Pseudo-code:
 *
 *     main([global, ...taskQueues])
 *
 *     main(roots)
 *       for each root
 *         move(root)
 *
 *       toProcess = nextHeap->bottom
 *       do 
 *         traverse(toProcess)
 *       while toProcess = toProcess->next
 *
 *     move(item)
 *       if not copied(item)
 *         copy item into new
 *         update movedTo of old
 *         clear movedTo in new
 *         update next pointer in new
 *
 *     traverse(item)
 *       for each obj reachable from item
 *          move(obj)
 *          updateReference(obj, item)
 *
 */

// roots: global environment
//        task queues pointing to ExecuableFunctions (which point to environments)
void _gcRun(JsValue** roots, uint64_t rootCount) {
    GcStats before = gcStats();

    // For item in roots
    for(uint64_t i = 0;
        i < rootCount;
        i++) {
        (JsValue*)mark((GcObject*)roots[i]);
    }
    log_info("GC fully traversed %llu roots", rootCount);

    for(char* toProcess = memory;
        toProcess->type != _UNITIALIZED_TYPE;
        toProcess += toProcess->size
        ) {
    }

    Heap* oldHeap = activeHeap;
    activeHeap = nextHeap;
    heapFree(oldHeap);
    nextHeap = oldHeap;

    GcStats after = gcStats();
    // TODO - do this more safely?
    int64_t saved = (int64_t)(before.used - after.used);
    log_info("GC complete, %lli bytes collected", saved);
}

void* _gcMovedTo(GcObject* object) {
    return object->movedTo;
}
