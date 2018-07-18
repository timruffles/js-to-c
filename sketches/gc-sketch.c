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

// packing this into 4 bytes wouldn't be sensible, as we'd
// only be able to have at max ~2-3GB
typedef struct AllocatedValue {
    // the diff between startOfHeapPointer and the value
    uint64_t heapLocation :62;
    bool marked :1;
    bool moved :1;
} AllocatedValue;

static char AREA_ONE[HEAP_SIZE()];
static char AREA_TWO[HEAP_SIZE()];

static AllocatedValue allocatedValues[1024 * 1024];
static AllocatedValue* nextValue = allocatedValues;

static char* activeArea = AREA_ONE;
static char* inactiveArea = AREA_TWO;

static void* startOfHeapPointer = AREA_ONE;
static void* endOfHeapPointer = AREA_ONE + HEAP_SIZE();

static char testString[] = "hello this is a string of text";

// algo
//   for each value pointer in envs
//      if moved
//        value = value->value.pointer
//      else
//        set mark 1
//        move 



typedef struct JsValue {
    char* type;
    union {
        double number;
        void* pointer;
    } value;
} JsValue;

// as we need to reserve heap space for the c string too,
// use the string pointer to allocate the string
typedef struct AllocatedString {
    JsValue* jsValue;
    char* string;
} AllocatedString;



uint64_t heapBytesRemaining() {
  // we know this'll be >= 0
  return (uint64_t)(endOfHeapPointer - startOfHeapPointer);
}

void GC() {
    printf("GC run\n");
    // TODO Walk env and mark
    uint64_t allocatedCount = max(nextValue - allocatedValues - 1, 0);
    /**
     * For each allocated value so far we check if:
     * - has been moved - nothing to do
     * - has been marked:
     *   - move value
     *   - update all pointers
     */
    for(uint64_t i = 0; i < allocatedCount; i++) {
        AllocatedValue* value = allocatedValues + i;
        if(value->moved) {
            continue;
        }
        if(value->marked) {
            // MOVE
            
        }
    }
}

void* allocate(uint64_t bytes) {
    if(heapBytesRemaining() < bytes) {
        GC();
    }
    void* pointer = startOfHeapPointer;
    memset(pointer, 0, bytes);
    startOfHeapPointer += bytes;
    return pointer;
}

JsValue* allocateJsValue() {
    JsValue* pointer = (JsValue*) allocate(sizeof(JsValue));
    *nextValue = (AllocatedValue) {
        .heapLocation = ((void*)pointer - startOfHeapPointer),
        .marked = 0,
        .moved = 0,
    };
    nextValue += 1;
    return pointer;
}

AllocatedString allocateJsString(char* cString, uint64_t length) {
    JsValue* valuePtr = allocateJsValue();
    char* stringPtr = (char*)allocate(length);

    return (AllocatedString) {
      .jsValue = valuePtr,
      .string = stringPtr,
    };
}

void mark(AllocatedValue* value) {
    value->marked = true;
}

int main() {
    memset(AREA_ONE, 0, HEAP_SIZE());
    memset(AREA_TWO, 0, HEAP_SIZE());

    uint64_t before = heapBytesRemaining();
    uint64_t toAllocate = 10;
    for(uint64_t i = 0; i < toAllocate; i++) {
        allocateJsString(testString, strlen(testString));
    }
    (*(allocatedValues + 1)).marked = true;
    (*(allocatedValues + 4)).marked = true;
    (*(allocatedValues + 6)).marked = true;

    printf("%i %i %i\n", before, heapBytesRemaining(), before - heapBytesRemaining());
}
