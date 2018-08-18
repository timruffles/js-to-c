#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct Heap {
    char* bottom;
    char* top;
    uint64_t size;

    bool expired;
} Heap;

void heapFree(Heap*);
Heap* heapCreate(uint64_t sizeInBytes);
void heapEmpty(Heap*);
void* heapAllocate(Heap*, uint64_t bytes);
void* heapTop(Heap*);

uint64_t heapBytesUsed(Heap*);
uint64_t heapBytesRemaining(Heap*);
