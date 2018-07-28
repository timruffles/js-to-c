#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KIBIBYTE() (1024)
#define MIBIBYTE() (KIBIBYTE() * KIBIBYTE())
#define HEAP_SIZE() (KIBIBYTE() * 1)
#define MAX(X, Y) (X > Y ? X : Y)

#define DEBUG_VAR(X, FMT) printf("%s var is "#FMT"\n", #X, X);


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
} JsValue;

typedef struct Env {
    JsValue** values;
    uint64_t count;
} Env;


// we track allocated values here - the nice thing
// about a separate tracking is we never have to update
// the location of existing values
// 
// TODO - is this too inefficient?:
// - if we have 1gb of heap we're tracking 1gb of allocated values, which unless they're all immediate values is overkill
// - how much of a faff is updating the pointers in the env values?
// - how much of this needs to be decided now, versus getting something
//   working and revisiting later?
//
// An array of pointer values to the heap
static JsValue* allocatedValues[HEAP_SIZE()];
JsValue** nextValue = allocatedValues;

// note: as each bool is a byte, this is wasteful,
// could be efficiently represented as an array of bitfields
static bool marks[HEAP_SIZE()];

// only envs we care about are current call env and all
// parents, parent env, and all callbacks in task queue
// (which link to various envs)
static JsValue* globalEnv;
static JsValue* callEnv;

uint64_t heapBytesUsed = 0;

static char testString[] = "hello this is a string of text";

uint64_t heapBytesRemaining() {
  // we know this'll be >= 0
  return HEAP_SIZE() - heapBytesUsed;
}

void freeValue(JsValue* value) {
    if(value->type == String || value->type == EnvType) {
        free(value->value.pointer);
        free(value);
    } else {
        // immediate
        free(value);
    }
}

bool* markForValue(JsValue* value) {
    // all JsValues live in the allocated values region, so
    // take diff from start and convert to bit offset in marks
    // array
    printf("value address %p vs start %p\n", value, *allocatedValues);
    ptrdiff_t diff = (void*)value - (void*)(allocatedValues);
    printf("index diff %lu\n", diff);
    return &marks[diff];
}

void markValue(JsValue* value) {
    printf("Marked %p\n", value);
    *markForValue(value) = true;
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

    // reset marks from previous runs
    memset(marks, 0, sizeof(marks));

    envWalk(globalEnv, markValue);
    envWalk(callEnv, markValue);

    printf("MARKING FINISHED\n");

    uint64_t allocatedCount = MAX(nextValue - allocatedValues - 1, 0);

    // TODO avoid GC'ing global + call env!

    /**
     * For each allocated value so far we check if:
     * - has been moved - nothing to do
     * - has been marked:
     *   - move value
     *   - update all pointers
     */
    printf("Allocated values at %p\n", allocatedValues);
    for(uint64_t i = 0; i < allocatedCount; i++) {
        JsValue** value = allocatedValues + i;
        if(marks[i] != true) {
            JsValue* onHeap = *value;
            printf("Freeing unmarked value at %llu %p - in alloc list at %p\n", i, onHeap, value);
            freeValue(onHeap);
        }
    }
    // TODO refill allocated values as we go
    // TODO reset marks array up to highest remaining element 
}


// allocates in the C heap
void* heapAllocate(uint64_t bytes) {
    if(heapBytesRemaining() < bytes) {
        GC();
    }
    heapBytesUsed += bytes;
    return calloc(sizeof(char), bytes);
}


JsValue* allocateJsValue(JsValueTypeForGC type) {
    JsValue* pointer = (JsValue*) heapAllocate(sizeof(JsValue));
    pointer->type = type;
    // I took a step towards the zen of pointers here
    *nextValue = pointer; // store the ValuePointer in the position pointed to by our nextValue pointer
    printf("Allocated a value at %p, stored in allocated values at %p\n", pointer, nextValue);
    nextValue += 1; // move nextValue pointer along, pointing at where we'll store the next allocated value
    return pointer;
}

JsValue* allocateJsString(char* cString, uint64_t length) {
    JsValue* valuePtr = allocateJsValue(String);
    char* stringPtr = heapAllocate(length);
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

int main() {
    printf("Tracking allocated values at %p\n", allocatedValues);
    uint64_t before = heapBytesRemaining();
    uint64_t toAllocate = 10;
    for(uint64_t i = 0; i < toAllocate; i++) {
        allocateJsString(testString, strlen(testString));
    }

    // setup a global + call env with some values
    JsValue** globalVals = calloc(3, sizeof(JsValue*));
    JsValue** callVals = calloc(2, sizeof(JsValue*));
    globalVals = (JsValue*[3]) {allocatedValues[1], allocatedValues[4], allocatedValues[6]};
    callVals = (JsValue*[2]){allocatedValues[7], allocatedValues[2]};
    globalVals[0] = allocatedValues[1];

    DEBUG_VAR(globalVals[0], %p);
    DEBUG_VAR(globalVals[1], %p);
    DEBUG_VAR(globalVals[2], %p);
    DEBUG_VAR(allocatedValues[1], %p);

    globalEnv = createEnv();
    callEnv = createEnv();
    envSetList(globalEnv, globalVals, 3);
    envSetList(callEnv, callVals, 2);

    GC();

    printf("%llu %llu %llu\n", before, heapBytesRemaining(), before - heapBytesRemaining());
}
