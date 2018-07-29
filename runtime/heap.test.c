#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "./language.h"
#include "./heap.h"
#include "./test.h"

static char testString[] = "hello I am a test string";

void allocateNExamplesOfSize(Heap* heap, uint64_t n, uint64_t size) {
    for(uint64_t i = 0; i < n; i++) {
        heapAllocate(heap, size);
    }
}

void itTracksAllocations() {
    Heap* heap = heapCreate(1024);
    uint64_t before = heapBytesRemaining(heap);
    allocateNExamplesOfSize(heap, 10, 8);
    uint64_t diff = before - heapBytesRemaining(heap);

    assert(diff == 80);

    assert(diff == heapBytesUsed(heap));
}

void itIsSafeToUseAllocatedValues() {
    Heap* heap = heapCreate(1024);
    double* dbl = heapAllocate(heap, sizeof(double));
    *dbl = 0.72;

    char testStr[] = "hello";
    char* string = heapAllocate(heap, sizeof(testStr));
    memcpy(string, testStr, sizeof(testStr));

    uint64_t* intPtr = heapAllocate(heap, sizeof(uint64_t));
    *intPtr = 42;

    assert(*dbl - 0.72 < 0.000001);
    assert(strcmp(testStr, string) == 0);
    assert(*intPtr == 42);
}


void itReturnsTheNullPointerIfOutOfSpace() {
    Heap* heap = heapCreate(60);
    assert(heapAllocate(heap, 1024) == NULL);
}

int main() {
    test(itTracksAllocations);
    test(itIsSafeToUseAllocatedValues);
    test(itReturnsTheNullPointerIfOutOfSpace);
}
