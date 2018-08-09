#pragma once
#include <stdint.h>
#include <stddef.h>

// movedTo: forwarding address during GC
// next:    next GC object
#define GcHeader void* movedTo; void* next; int type;

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
GcStats gcStats(void);

void _gcTestInit(void);
