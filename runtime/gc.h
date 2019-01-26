#pragma once
#include <stdint.h>
#include <stddef.h>

#include "gcObject.h"
#include "language.h"
#include "config.h"

typedef uint64_t GcAtomicId;

typedef struct {
    uint64_t size;
} GcConfig;

void gcInit(Config*);
void* gcAllocate(size_t, int type);

GcAtomicId gcAtomicGroupStart(void);
void gcAtomicGroupEnd(GcAtomicId);

void gcStartProtectAllocations(void);
void gcStopProtectAllocations(void);

void _gcTestInit(Config*);

void _gcRunGlobal(void);
void _gcRun(JsValue** roots, uint64_t rootCount);
void _gcVisualiseHeap(void);

typedef void* (GcCallback)(void*);
