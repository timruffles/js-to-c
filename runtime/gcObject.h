#include <stdbool.h>

#pragma once
typedef struct GcObject GcObject;

// movedTo: forwarding address during GC
// next:    next GC object
#define GcHeader uint64_t size; int type; boolean marked;

// common head of all Gc allocated structs
typedef struct GcObject {
    GcHeader;
} GcObject;
