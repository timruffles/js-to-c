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
    JsValue* values;
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
static bool marks[HEAP_SIZE()];
JsValue** nextValue = allocatedValues;

// only envs we care about are current call env and all
// parents, parent env, and all callbacks in task queue
// (which link to various envs)
static Env globalEnv[3];
static Env callEnv[2];

uint64_t heapBytesUsed = 0;

static char testString[] = "hello this is a string of text";

uint64_t heapBytesRemaining() {
  // we know this'll be >= 0
  return HEAP_SIZE() - heapBytesUsed;
}

void freeValue(JsValue* value) {
    if(value->type == String) {
        free(value->value.pointer);
        free(value);
    } else if(value->type == EnvType) {
        // free object etc
    } else {
        // immediate
        free(value);
    }
}

void GC() {
    printf("GC run\n");
    // TODO Walk global and currentEnv and mark
    uint64_t allocatedCount = MAX(nextValue - allocatedValues - 1, 0);
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
        if(!marks[i]) {
            JsValue* onHeap = *value;
            printf("Freeing unmarked value at %llu %p - in alloc list at %p\n", i, onHeap, value);
            freeValue(onHeap);
        }
    }
    // TODO refill allocated values as we go
    // TODO reset marks array up to highest remaining element 
}

void* heapAllocate(uint64_t bytes) {
    if(heapBytesRemaining() < bytes) {
        GC();
    }
    heapBytesUsed += bytes;
    return calloc(sizeof(char), bytes);
}


JsValue* allocateJsValue(JsValueTypeForGC type) {
    JsValue* pointer = (JsValue*) heapAllocate(sizeof(JsValue));
    printf("Allocated a value at %p\n", pointer);
    pointer->type = type;
    *nextValue = pointer;
    nextValue += 1;
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

void envWalk(JsValue* value, void (callback)(JsValue*)) {
    Env* env = (Env*) value->value.pointer;
    for(uint64_t i = 0; i < env->count; i++) {
        JsValue* found = env->values + i;
        callback(found);
    }
}

int main() {
    uint64_t before = heapBytesRemaining();
    uint64_t toAllocate = 10;
    for(uint64_t i = 0; i < toAllocate; i++) {
        allocateJsString(testString, strlen(testString));
    }
    GC();

    printf("%i %i %i\n", before, heapBytesRemaining(), before - heapBytesRemaining());
}
