
void allocateNExamplesOfSize(uint64_t n, uint64_t size) {
    for(uint64_t i = 0; i < n; i++) {
        heapAllocate(activeHeap, size);
    }
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
