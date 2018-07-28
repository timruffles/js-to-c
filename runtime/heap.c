#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Heap {
    char* bottom;
    char* top;
    uint64_t size;
} Heap;

uint64_t heapBytesUsed(Heap* heap) {
    // we know this'll be >= 0
    return heap->top - heap->bottom;
}

uint64_t heapBytesRemaining(Heap* heap) {
    // we know this'll be >= 0
    return heap->size - heapBytesUsed(heap);
}

void heapFree(Heap* heap) {
    free(heap->bottom);
}

void heapInit(Heap* heap, uint64_t size) {
    heap->bottom = calloc(1, size);
    heap->top = heap->bottom;
    heap->size = size; 
}

void heapEmpty(Heap* heap) {
    heap->top = heap->bottom;
    // maybe fill with 0xDEAD?
}

void* heapAllocate(Heap* heap, uint64_t bytes) {
    if(heapBytesRemaining(heap) < bytes) {
        if(isActiveHeap(heap)) {
            gcRun();
        } else {
            assert("heap exhausted" == NULL);
        }
    }
    
    char* oldTop = heap->top;
    heap->top += bytes;
    return oldTop;
}

void moveValue(JsValue* value) {
    if(isMoved(value)) {
        return;
    }

    JsValue* target;
    target = heapAllocate(nextHeap, sizeof(JsValue));
    *target = *value;
    // TODO - move structured values
    if(value->type == String) {
        uint64_t stringLen = strlen(value->value.pointer);
        char* stringLocation = heapAllocate(nextHeap, stringLen);
        memcpy(stringLocation, value->value.pointer, stringLen);
        target->value.pointer = stringLocation;
    }
    value->movedTo = target;
}
