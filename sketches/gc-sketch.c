#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../runtime/test.h"

#define KIBIBYTE() (1024)
#define MIBIBYTE() (KIBIBYTE() * KIBIBYTE())
#define HEAP_SIZE() (MIBIBYTE() * 1)
#define MAX(X, Y) (X > Y ? X : Y)

#define DEBUG_VAR(X, FMT) printf("%s var is "#FMT"\n", #X, X);
#define DEBUG_CALL(X) printf(#X" before\n"); X; printf(#X" after\n");

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef enum JsValueTypeForGC {
    // pointer - just string in this example
    String,
    // immediate: primitive, booleans
    Number,
    EnvType,
} JsValueTypeForGC;

typedef struct JsValue {
    JsValueTypeForGC type;
    union {
        // immediate type (only number in our example)
        double number;
        // pointer types
        void* pointer;
    } value;

    struct JsValue* movedTo;
} JsValue;

typedef struct Env {
    JsValue** values;
    uint64_t size;
} Env;

typedef struct Heap {
    char* bottom;
    char* top;
    uint64_t size;
} Heap;

static Heap heapOne;
static Heap heapTwo;
static Heap* activeHeap = &heapOne;
static Heap* nextHeap = &heapTwo;

/**
 * The roots from which we trace live objects in the system.
 *
 * In future, there'd also be the current stack frame (with all parents)
 * and task queues. 
 */
static JsValue* globalEnv;

static char testString[] = "hello this is a string of text";

void gcRun(void);
void moveValue(JsValue*);

uint64_t heapBytesUsed(Heap* heap) {
    // we know this'll be >= 0
    return heap->top - heap->bottom;
}

uint64_t heapBytesRemaining(Heap* heap) {
    // we know this'll be >= 0
    return heap->size - heapBytesUsed(heap);
}

void heapFree(Heap* heap) {
    free(heap->bottom);
}

void heapInit(Heap* heap, uint64_t size) {
    heap->bottom = calloc(1, size);
    heap->top = heap->bottom;
    heap->size = size; 
}

bool isMoved(JsValue* value) {
    return value->movedTo != NULL;
}


void envWalk(JsValue* value, void (callback)(JsValue*)) {
    Env* env = (Env*) value->value.pointer;
    for(uint64_t i = 0; i < env->size; i++) {
        JsValue* found = env->values[i];
        callback(found);
        env->values[i] = found->movedTo;
    }
}

bool isActiveHeap(Heap* heap) {
    return heap == activeHeap;
}

void heapEmpty(Heap* heap) {
    heap->top = heap->bottom;
    // maybe fill with 0xDEAD?
}

void* heapAllocate(Heap* heap, uint64_t bytes) {
    if(heapBytesRemaining(heap) < bytes) {
        if(isActiveHeap(heap)) {
            gcRun();
        } else {
            assert("heap exhausted" == NULL);
        }
    }
    
    char* oldTop = heap->top;
    heap->top += bytes;
    return oldTop;
}

JsValue* heapAllocateJsValue(Heap* heap, JsValueTypeForGC type) {
    JsValue* pointer = (JsValue*) heapAllocate(heap, sizeof(JsValue));
    pointer->type = type;
    return pointer;
}

JsValue* allocateJsValue(JsValueTypeForGC type) {
    return heapAllocateJsValue(activeHeap, type);
}

JsValue* heapAllocateJsString(Heap* heap, char* cString, uint64_t lengthIncludingNull) {
    JsValue* valuePtr = allocateJsValue(String);
    char* stringPtr = heapAllocate(heap, lengthIncludingNull);
    memcpy(stringPtr, cString, lengthIncludingNull);
    valuePtr->value.pointer = stringPtr;
    return valuePtr;
}

JsValue* allocateJsString(char* cString, uint64_t lengthIncludingNull) {
    return heapAllocateJsString(activeHeap, cString, lengthIncludingNull);
}

JsValue* envCreate() {
    JsValue* value = allocateJsValue(EnvType);
    value->value.pointer = heapAllocate(activeHeap, sizeof(Env));
    *((Env*)value->value.pointer) = (Env) {
        .values = NULL,
        .size = 0,
    };
    return value;
}

void envSetList(JsValue* value, JsValue** list, uint64_t size) {
    Env* env = (Env*) value->value.pointer;
    env->size = size;
    env->values = list;
}

void gcInit() {
    heapInit(&heapOne, HEAP_SIZE());
    heapInit(&heapTwo, HEAP_SIZE());
}

void gcReset() {
    heapFree(&heapOne);
    heapFree(&heapTwo);
    gcInit();
}

void moveValue(JsValue* value) {
    printf("moving %p\n", value);
    if(isMoved(value)) {
        return;
    }

    JsValue* target;
    target = heapAllocateJsValue(nextHeap, value->type);
    *target = *value;
    if(value->type == String) {
        uint64_t stringLen = strlen(value->value.pointer);
        char* stringLocation = heapAllocate(nextHeap, stringLen);
        memcpy(stringLocation, value->value.pointer, stringLen);
        target->value.pointer = stringLocation;
    }
    value->movedTo = target;
}

void gcRun() {
    printf("GC run\n");

    moveValue(globalEnv);
    globalEnv = globalEnv->movedTo;

    envWalk(globalEnv, moveValue);

    Heap* oldActive = activeHeap;
    activeHeap = nextHeap;
    nextHeap = oldActive;

    heapEmpty(oldActive);
}


/**
 * Tests
 */

void allocateNExamplesOfSize(uint64_t n, uint64_t size) {
    for(uint64_t i = 0; i < n; i++) {
        heapAllocate(activeHeap, size);
    }
}

void itDoesNotMoveGarbage() {
    gcReset();

    allocateJsValue(Number);
    allocateJsValue(Number);
    allocateJsValue(Number);
    JsValue* liveOne = allocateJsValue(Number);
    JsValue* liveTwo = allocateJsValue(Number);

    JsValue* globalVals[] = {liveOne, liveTwo};
    globalEnv = envCreate();
    envSetList(globalEnv, globalVals, ARRAY_SIZE(globalVals));

    uint64_t before = heapBytesUsed(activeHeap);
    gcRun();
    uint64_t after = heapBytesUsed(activeHeap);

    assert(after < before);
}

void itMovesGlobalInGc() {
    gcReset();

    // this is a pointer on old heap
    Env* initialGlobal = envCreate();
    globalEnv = initialGlobal;
    gcRun();

    assert(globalEnv != initialGlobal);
    assert(globalEnv->movedTo != NULL);
}

void itMovesLiveObjects() {
    gcReset();

    JsValue* liveOne = allocateJsValue(Number);
    allocateJsValue(Number);
    JsValue* liveTwo = allocateJsValue(Number);
    allocateJsValue(Number);
    allocateJsValue(Number);
    JsValue* liveReferencedMultipleTimes = allocateJsValue(Number);

    JsValue* globalVals[] = {liveOne, liveTwo, liveReferencedMultipleTimes, liveReferencedMultipleTimes};
    globalEnv = envCreate();
    envSetList(globalEnv, globalVals, ARRAY_SIZE(globalVals));

    assert(globalVals[0] == liveOne);
    gcRun();

    // we're not pointing at new 
    assert(globalVals[0] != liveOne);

    // ensure we have indeed copied the value, by demoing writes to old value don't affect new
    liveOne->type = String;
    assert(globalVals[0]->type == Number);

    // and that we're pointing to the new value
    assert(liveOne->movedTo == globalVals[0]);

    // multiple references are fine - we don't move things twice, and update the pointers
    // to ensure they're moved
    assert(liveReferencedMultipleTimes->movedTo == globalVals[2]);
    assert(liveReferencedMultipleTimes->movedTo == globalVals[3]);

}

void itMovesValuesOnceOnly() {
    JsValue* value = allocateJsString(testString, sizeof(testString));
    moveValue(value);
    uint64_t before = heapBytesRemaining(nextHeap);
    moveValue(value);
    moveValue(value);

    assert(before == heapBytesRemaining(nextHeap));
}

void itMovesStrings() {
    JsValue* value = allocateJsString(testString, sizeof(testString));
    moveValue(value);
    assert(value->value.pointer != value->movedTo->value.pointer);
    assert(strcmp(value->value.pointer, value->movedTo->value.pointer) == 0);
}

void itMovesImmediateValues() {
    JsValue* value = allocateJsValue(Number);
    moveValue(value);
    assert(value->movedTo != value);
    assert(isMoved(value));
    assert(value->movedTo->type == Number);
}

void itAllocatesJsStrings() {
    JsValue* value = allocateJsString(testString, sizeof(testString));
    assert(value->type == String);
    assert(strncmp(value->value.pointer, testString, sizeof(testString)) == 0);
}

void itAllocatesJsValues() {
    JsValue* value = allocateJsValue(Number);
    assert(value->type == Number);
}

void itIsSafeToUseAllocatedValues() {
    double* dbl = heapAllocate(activeHeap, sizeof(double));
    *dbl = 0.72;

    char testStr[] = "hello";
    char* string = heapAllocate(activeHeap, sizeof(testStr));
    memcpy(string, testStr, sizeof(testStr));

    uint64_t* intPtr = heapAllocate(activeHeap, sizeof(uint64_t));
    *intPtr = 42;

    assert(*dbl - 0.72 < 0.000001);
    assert(strcmp(testStr, string) == 0);
    assert(*intPtr == 42);
}

void itTracksAllocations() {
    uint64_t before = heapBytesRemaining(activeHeap);
    allocateNExamplesOfSize(10, 8);
    uint64_t diff = before - heapBytesRemaining(activeHeap);
    assert(diff == 80); 
}

int main() {
    DEBUG_CALL(gcInit());

    test(itTracksAllocations);
    test(itIsSafeToUseAllocatedValues);
    test(itAllocatesJsValues);
    test(itAllocatesJsStrings);
    test(itMovesImmediateValues);
    test(itMovesStrings);
    test(itMovesValuesOnceOnly);
    test(itMovesLiveObjects);
    test(itDoesNotMoveGarbage);
}
