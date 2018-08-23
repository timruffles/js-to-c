#pragma once
#include <stdint.h>
#include <stddef.h>

#include "gcObject.h"
#include "language.h"


typedef struct {
    uint64_t used;
    uint64_t remaining;
    uint64_t heapSize;
} GcStats;

void gcInit(void);
void* gcAllocate(size_t, int type);
GcStats gcStats(void);

void _gcTestInit(void);

void _gcRun(JsValue** roots, uint64_t rootCount);
void _gcVisualiseHeap(void);

typedef void* (GcCallback)(void*);
