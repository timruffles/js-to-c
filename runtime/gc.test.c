

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

