#pragma once
#include <stdint.h>
#include <stddef.h>

#include "gcObject.h"
#include "language.h"
#include "config.h"

typedef int GcAtomicId;

typedef struct {
    uint64_t size;
} GcConfig;

typedef struct {
    uint64_t used;
    uint64_t remaining;
    uint64_t heapSize;
} GcStats;

void gcInit(Config*);
void* gcAllocate(size_t, int type);
GcStats gcStats(void);

GcAtomicId gcAtomicGroupStart(void);
void gcAtomicGroupEnd(GcAtomicId);

void _gcTestInit(Config*);

void _gcRunGlobal(void);
void _gcRun(JsValue** roots, uint64_t rootCount);
void _gcVisualiseHeap(void);

typedef void* (GcCallback)(void*);
