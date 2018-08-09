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
    GcObject* allocated = heapAllocate(activeHeap, bytes);
    if(allocated == NULL) {
        log_err("GC REQUIRED - TODO");
    }
    allocated->next = heapTop(activeHeap);
    return allocated;
}

GcStats gcStats() {
    return (GcStats) {
        .used = heapBytesUsed(activeHeap),
        .remaining = heapBytesRemaining(activeHeap),
    };
}

// roots: global environment
//        task queues pointing to ExecuableFunctions (which point to environments)
//void gcRun(GcObject* roots, uint64_t rootCount) {
//    // For item in roots
//    for(uint64_t i = 0;
//        i < rootCount;
//        i++) {
//
//
//    }
//    //   move item
//    //
//    // While to-check < top
//    //   traverse item
//    //     move found into to-check
//    //
//    // Done
//    //
//    // Move(item)
//    //   copy item into new
//    //     update movedTo of old
//    //     clear movedTo in new
//    //     update next pointer in new
//    //   traverse item
//    //     move found into to-check area
//    
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
