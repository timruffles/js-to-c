#include <stdint.h>
#include <stddef.h>

#include "heap.h"
#include "lib/debug.h"
#include "gc.h"
#include "language.h"
#include "assert.h"
#include "objects.h"

#define HEAP_SIZE (1024 * 1024)

static Heap heapOne;
static Heap heapTwo;

static Heap* activeHeap;
static Heap* nextHeap;

void gcInit() {
    activeHeap = heapCreate(HEAP_SIZE);
    nextHeap = heapCreate(HEAP_SIZE);
}

void _gcTestInit() {
    if(activeHeap != NULL) {
        heapFree(activeHeap);
        heapFree(nextHeap);
    }
    gcInit();
}

void* gcAllocate2(size_t bytes, int type) {
    GcObject* allocated = gcAllocate(bytes);
    allocated->type = type;
    return allocated;
}

void* gcAllocate(size_t bytes) {
    GcObject* allocated = heapAllocate(activeHeap, bytes);
    if(allocated == NULL) {
        // TODO
    }
    allocated->next = heapTop(activeHeap);
    assert(allocated->next != NULL);
    return allocated;
}

GcStats gcStats() {
    return (GcStats) {
        .used = heapBytesUsed(activeHeap),
        .remaining = heapBytesRemaining(activeHeap),
    };
}

static uint64_t gcObjectSize(GcObject* object) {
    assert(object->next != NULL);
    return (uint64_t)object->next - (uint64_t)object;
}

static GcObject* move(GcObject* item) {
    if(item->movedTo) {
        return item->movedTo;
    }

    uint64_t size = gcObjectSize(item);
    GcObject* newAddress = heapAllocate(nextHeap, size);
    memcpy(newAddress, item, size);
    item->movedTo = newAddress;
    newAddress->next = (void*)(((uint64_t)newAddress) + size);
    return newAddress;
}

static void traverse(GcObject* object) {
    switch(object->type) {
        case OBJECT_TYPE:
            return objectGcTraverse((void*)object, (void*)move);
        // TODO - strings copy over string
        default:
            return;
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
void _gcRun(GcObject** roots, uint64_t rootCount) {
    GcStats before = gcStats();

    // For item in roots
    for(uint64_t i = 0;
        i < rootCount;
        i++) {
        roots[i] = move(roots[i]);
    }
    log_info("GC moved %llu roots", rootCount);

    GcObject* toProcess = (void*)nextHeap->bottom;
    while((uint64_t*)toProcess != (uint64_t*)nextHeap->top) {
        traverse(toProcess);
        toProcess = toProcess->next;
    }

    Heap* oldHeap = activeHeap;
    activeHeap = nextHeap;
    heapFree(oldHeap);
    nextHeap = oldHeap;

    GcStats after = gcStats();
    int saved = before.used - after.used;
    log_info("GC complete, %i bytes collected", saved);
}

void* _gcMovedTo(GcObject* object) {
    return object->movedTo;
}
