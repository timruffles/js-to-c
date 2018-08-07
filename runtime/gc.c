#include <stdint.h>
#include <stddef.h>

#include "heap.h"
#include "lib/debug.h"
#include "gc.h"

#define HEAP_SIZE (1024 * 1024)

//static Heap heapOne;
//static Heap heapTwo;

static Heap* activeHeap;

//static Heap* nextHeap = &heapTwo;
//
void gcInit() {
    activeHeap = heapCreate(HEAP_SIZE);
}

void _gcTestInit() {
    if(activeHeap != NULL) {
        heapFree(activeHeap);
    }
    gcInit();
}

void* gcAllocate(size_t bytes) {
    void* allocated = heapAllocate(activeHeap, bytes);
    if(allocated == NULL) {
        log_err("GC REQUIRED - TODO");
    }
    return allocated;
}

GcStats gcStats() {
    return (GcStats) {
        .used = heapBytesUsed(activeHeap),
        .remaining = heapBytesRemaining(activeHeap),
    };
}

//void gcRun() {
//    printf("GC run\n");
//
//    moveValue(globalEnv);
//    globalEnv = globalEnv->movedTo;
//
//    envWalk(globalEnv, moveValue);
//
//    Heap* oldActive = activeHeap;
//    activeHeap = nextHeap;
//    nextHeap = oldActive;
//
//    heapEmpty(oldActive);
//}
//
//void gcReset() {
//    heapFree(&heapOne);
//    heapFree(&heapTwo);
//    gcInit();
//}
//
////void envWalk(JsValue* value, void (callback)(JsValue*)) {
////    Env* env = (Env*) value->value.pointer;
////    for(uint64_t i = 0; i < env->size; i++) {
////        JsValue* found = env->values[i];
////        callback(found);
////        env->values[i] = found->movedTo;
////    }
////}
//
//void moveValue(JsValue* value) {
//    if(isMoved(value)) {
//        return;
//    }
//
//    JsValue* target;
//    target = heapAllocate(nextHeap, sizeof(JsValue));
//    *target = *value;
//    // TODO - move structured values
//    if(value->type == String) {
//        uint64_t stringLen = strlen(value->value.pointer);
//        char* stringLocation = heapAllocate(nextHeap, stringLen);
//        memcpy(stringLocation, value->value.pointer, stringLen);
//        target->value.pointer = stringLocation;
//    }
//    value->movedTo = target;
//}
