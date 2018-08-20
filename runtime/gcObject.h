#pragma once
typedef struct GcObject GcObject;

// movedTo: forwarding address during GC
// next:    next GC object
#define GcHeader GcObject* movedTo; GcObject* next; int type

// common head of all Gc allocated structs
typedef struct GcObject {
    GcHeader;
} GcObject;
