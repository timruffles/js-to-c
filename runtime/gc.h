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
void* _gcAllocate(size_t, int type);

GcAtomicId gcAtomicGroupStart(void);
void gcAtomicGroupEnd(GcAtomicId);
void gcOnExceptionsThrow(void);

void gcStartProtectAllocations(void);
void gcStopProtectAllocations(void);

void _gcTestInit(Config*);

void _gcRunGlobal(void);
void _gcRun(JsValue** roots, uint64_t rootCount);

typedef struct {
  GcObject* highlight;
} GcVisualiseHeapOpts;
void _gcVisualiseHeap(GcVisualiseHeapOpts*);

typedef void* (GcCallback)(void*);
