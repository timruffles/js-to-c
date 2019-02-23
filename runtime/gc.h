#pragma once
#include <stdint.h>
#include <stddef.h>

#include "gcObject.h"
#include "language.h"
#include "config.h"


typedef struct {
    uint64_t size;
} GcConfig;

// used to track intermediate values exempt from GC 
typedef struct GcProtectedValue {
    struct GcProtectedValue* next;
    GcObject* value;
} GcProtectedValue; 

void gcInit(Config*);
void* gcAllocate(size_t, int type);
void* _gcAllocate(size_t, int type);

void gcProtectValue(JsValue*);
void gcUnprotectValues(uint64_t count);

void gcStartProtectAllocations(void);
void gcStopProtectAllocations(void);

void _gcTestInit(Config*);

void _gcRunGlobal(void);
void _gcRun(JsValue** roots, uint64_t rootCount);

typedef struct {
  GcObject* highlight;
} GcVisualiseHeapOpts;
void _gcVisualiseHeap(GcVisualiseHeapOpts*);

typedef void (GcCallback)(void*);
