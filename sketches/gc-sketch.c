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
    uint64_t count;
} Env;


static char* activeArea;
static char* nextArea;
static char* heapTop;

static JsValue* globalEnv;
static JsValue* callEnv;

static char testString[] = "hello this is a string of text";

uint64_t heapBytesRemaining() {
    // we know this'll be >= 0
    return HEAP_SIZE() - (heapTop - activeArea);
}

void markValue(JsValue* value) {
}

void envWalk(JsValue* value, void (callback)(JsValue*)) {
    Env* env = (Env*) value->value.pointer;
    for(uint64_t i = 0; i < env->count; i++) {
        JsValue* found = env->values[i];
        callback(found);
    }
}

void GC() {
    printf("GC run\n");

    envWalk(globalEnv, markValue);
    envWalk(callEnv, markValue);
}


// allocates in the C heap
void* heapAllocate(uint64_t bytes) {
    if(heapBytesRemaining() < bytes) {
        GC();
    }
    
    char* oldTop = heapTop;
    heapTop += bytes;
    return oldTop;
}


JsValue* allocateJsValue(JsValueTypeForGC type) {
    JsValue* pointer = (JsValue*) heapAllocate(sizeof(JsValue));
    pointer->type = type;
    return pointer;
}

JsValue* allocateJsString(char* cString, uint64_t lengthIncludingNull) {
    JsValue* valuePtr = allocateJsValue(String);
    char* stringPtr = heapAllocate(lengthIncludingNull);
    memcpy(stringPtr, cString, lengthIncludingNull);
    valuePtr->value.pointer = stringPtr;
    return valuePtr;
}

JsValue* createEnv() {
    JsValue* value = allocateJsValue(EnvType);
    value->value.pointer = heapAllocate(sizeof(Env));
    *((Env*)value->value.pointer) = (Env) {
        .values = NULL,
        .count = 0,
    };
    return value;
}

void envSetList(JsValue* value, JsValue** list, uint64_t count) {
    Env* env = (Env*) value->value.pointer;
    env->count = count;
    env->values = list;
}

void heapInit() {
    activeArea = calloc(1, HEAP_SIZE());
    nextArea = calloc(1, HEAP_SIZE());

    heapTop = activeArea;
}


/**
 * Tests
 */

void allocateNExamplesOfSize(uint64_t n, uint64_t size) {
    for(uint64_t i = 0; i < n; i++) {
        heapAllocate(size);
    }
}

void itIsSafeToUseAllocatedValues() {
    double* dbl = heapAllocate(sizeof(double));
    *dbl = 0.72;

    char testStr[] = "hello";
    char* string = heapAllocate(sizeof(testStr));
    memcpy(string, testStr, sizeof(testStr));

    uint64_t* intPtr = heapAllocate(sizeof(uint64_t));
    *intPtr = 42;

    assert(*dbl - 0.72 < 0.000001);
    assert(strcmp(testStr, string) == 0);
    assert(*intPtr == 42);
}

void itTracksAllocations() {
    uint64_t before = heapBytesRemaining();
    allocateNExamplesOfSize(10, 8);
    uint64_t diff = before - heapBytesRemaining();
    assert(diff == 80); 
}

int main() {
    DEBUG_CALL(heapInit());

    test(itTracksAllocations);
    test(itIsSafeToUseAllocatedValues);
}
