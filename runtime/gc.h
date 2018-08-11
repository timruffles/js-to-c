#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct GcObject GcObject;

// movedTo: forwarding address during GC
// next:    next GC object
#define GcHeader GcObject* movedTo; GcObject* next; int type

// common head of all Gc allocated structs
typedef struct GcObject {
    GcHeader;
} GcObject;

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

void _gcRun(GcObject** roots, uint64_t rootCount);

typedef void* (GcCallback)(void*);
