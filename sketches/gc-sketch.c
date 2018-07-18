#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// 100 mebibytes
#define HEAP_SIZE() (1024 * 1024 * 128)

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

static AllocatedValue* allocatedValues[1024 * 1024];

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
//        
//

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
    return (JsValue*) allocate(sizeof(JsValue));
}

AllocatedString allocateJsString(char* cString, uint64_t length) {
    JsValue* valuePtr = allocateJsValue();
    char* stringPtr = (char*)allocate(length);

    return (AllocatedString) {
      .jsValue = valuePtr,
      .string = stringPtr,
    };
}

int main() {
    memset(AREA_ONE, 0, HEAP_SIZE());
    memset(AREA_TWO, 0, HEAP_SIZE());

    uint64_t before = heapBytesRemaining();
    allocateJsString(testString, strlen(testString));

    printf("%i %i %i\n", before, heapBytesRemaining(), before - heapBytesRemaining());
}
