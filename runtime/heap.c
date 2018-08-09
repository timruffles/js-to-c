#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./conditions.h"
#include "./lib/debug.h"

#define ENSURE_CALLOC_BYTES(V, N) V = calloc(N, 1); assert(V != NULL);

typedef struct Heap {
    char* bottom;
    char* top;
    uint64_t size;
} Heap;

uint64_t heapBytesUsed(Heap* heap) {
    assert_debug(heap != NULL);
    // we know this'll be >= 0
    return heap->top - heap->bottom;
}

uint64_t heapBytesRemaining(Heap* heap) {
    assert_debug(heap != NULL);
    // we know this'll be >= 0
    return heap->size - heapBytesUsed(heap);
}

void heapFree(Heap* heap) {
    assert_debug(heap != NULL);
    free(heap->bottom);
}

Heap* heapCreate(uint64_t size) {
    Heap* heap;
    ENSURE_CALLOC_BYTES(heap, sizeof(Heap));
    ENSURE_CALLOC_BYTES(heap->bottom, size);
    heap->size = size;
    heap->top = heap->bottom;
    return heap;
}

void heapEmpty(Heap* heap) {
    assert_debug(heap != NULL);
    heap->top = heap->bottom;
}

void* heapAllocate(Heap* heap, uint64_t bytes) {
    assert_debug(heap != NULL);
    if(heapBytesRemaining(heap) < bytes) {
        return NULL;
    }

    char* oldTop = heap->top;
    heap->top += bytes;
    return oldTop;
}

void* heapTop(Heap* heap) {
    return heap->top;
}
