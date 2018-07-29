#pragma once
#include <stdint.h>

typedef struct Heap Heap;

void heapFree(Heap*);
Heap* heapCreate(uint64_t sizeInBytes);
void heapEmpty(Heap*);
void* heapAllocate(Heap*, uint64_t bytes);

uint64_t heapBytesUsed(Heap*);
uint64_t heapBytesRemaining(Heap*);

