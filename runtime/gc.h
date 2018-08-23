#pragma once
#include <stdint.h>
#include <stddef.h>

#include "gcObject.h"
#include "language.h"


typedef struct {
    uint64_t used;
    uint64_t remaining;
} GcStats;

void gcInit(void);
void* gcAllocate(size_t);
void* gcAllocate2(size_t, int type);
GcStats gcStats(void);

void _gcTestInit(void);
void* _gcMovedTo(GcObject*);

void _gcRun(JsValue** roots, uint64_t rootCount);

typedef void* (GcCallback)(void*);
