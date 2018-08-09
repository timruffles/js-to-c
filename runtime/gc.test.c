#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "gc.h"
#include "language.h"
#include "objects.h"
#include "strings.h"

#define ASSERT_MOVED(P) assert(_gcMovedTo((void*)P) != NULL)
#define ASSERT_NOT_MOVED(P) assert(_gcMovedTo((void*)P) == NULL)
#define MOVED(V) _gcMovedTo((void*)V)

#define JS_SET(O,P,V) objectSet(O,stringCreateFromCString(P),V)
#define JS_GET(O,P) objectGet(O,stringCreateFromCString(P))

typedef struct {
    GcHeader;
    uint64_t number;
} TestStruct;

void itCanTestInitWithoutInit() {
    _gcTestInit();
}

void itCanTestInitAfterInit() {
    // this inits twice, freeing first time
    _gcTestInit();
    _gcTestInit();
}

void itCanAllocate() {
    _gcTestInit();
    void* value = gcAllocate(sizeof(uint64_t)); 
    assert(value != NULL);
    // should be zeroed
    uint64_t* values = value;
    assert(*values == 0);
}

void itSetsNextPointerToPositionOfObjectAllocatedNext() {
    _gcTestInit();

    TestStruct* valueA = gcAllocate(sizeof(TestStruct)); 
    TestStruct* valueB = gcAllocate(sizeof(TestStruct)); 
    valueB->number = 42;

    assert(((TestStruct*)valueA->next)->number == 42);
}

void itGarbageCollectsCorrectly() {
    _gcTestInit();

    JsValue* liveOne = objectCreatePlain();
    JsValue* garbageOne = objectCreatePlain();
    JsValue* liveTwo = objectCreatePlain();
    JsValue* garbageTwo = objectCreatePlain();
    JsValue* liveDeepOne = objectCreatePlain();

    JsValue* root = objectCreatePlain();
    JS_SET(root, "liveOne", liveOne);
    JS_SET(liveOne, "id", jsValueCreateNumber(101));
    JS_SET(root, "liveTwo", liveTwo);
    JS_SET(root, "liveOneSecondRef", liveOne);

    JS_SET(liveTwo, "liveDeepOne", liveDeepOne);
    JS_SET(liveDeepOne, "id", jsValueCreateNumber(2002));

    GcObject* roots[] = {(void*)root};
    _gcRun(roots, 1);

    JsValue* newRoot = _gcMovedTo((void*)root);

    // we copied over roots + live
    ASSERT_MOVED(root);
    ASSERT_MOVED(liveOne);
    ASSERT_MOVED(liveTwo);
    ASSERT_MOVED(liveDeepOne);

    // ensure objects have been copied over safely
    assert(JS_GET(MOVED(liveTwo), "liveDeepOne") == MOVED(liveDeepOne));
    assert(jsValueNumber(JS_GET(JS_GET(MOVED(liveTwo), "liveDeepOne"), "id")) == 2002);

    // ensure we have indeed copied the value, by demoing writes to old value don't affect new
    JS_SET(liveOne, "id", jsValueCreateNumber(42));
    assert((jsValueNumber(JS_GET(liveOne, "id")) - 42) < 0.00001);
    assert((jsValueNumber(JS_GET(MOVED(liveOne), "id")) - 101) < 0.00001);

    // multiple references are fine - we don't move things twice, and update the pointers
    // to ensure they're moved
    assert(JS_GET(MOVED(root), "liveOne")
        == JS_GET(MOVED(root), "liveOneSecondRef"));

    // didn't copy over garbage
    ASSERT_NOT_MOVED(garbageOne);
    ASSERT_NOT_MOVED(garbageTwo);
}


int main() {
    test(itCanTestInitWithoutInit); 
    test(itCanTestInitAfterInit); 
    test(itCanAllocate); 
    test(itSetsNextPointerToPositionOfObjectAllocatedNext);

    test(itGarbageCollectsCorrectly);
}

//void itDoesNotMoveGarbage() {
//    gcReset();
//
//    allocateJsValue(Number);
//    allocateJsValue(Number);
//    allocateJsValue(Number);
//    JsValue* liveOne = allocateJsValue(Number);
//    JsValue* liveTwo = allocateJsValue(Number);
//
//    JsValue* globalVals[] = {liveOne, liveTwo};
//    globalEnv = envCreate();
//    envSetList(globalEnv, globalVals, ARRAY_SIZE(globalVals));
//
//    uint64_t before = heapBytesUsed(activeHeap);
//    gcRun();
//    uint64_t after = heapBytesUsed(activeHeap);
//
//    assert(after < before);
//}
//
//
//
//void itMovesValuesOnceOnly() {
//    JsValue* value = allocateStringData(testString, sizeof(testString));
//    moveValue(value);
//    uint64_t before = heapBytesRemaining(nextHeap);
//    moveValue(value);
//    moveValue(value);
//
//    assert(before == heapBytesRemaining(nextHeap));
//}
//
//void itMovesStrings() {
//    JsValue* value = allocateStringData(testString, sizeof(testString));
//    moveValue(value);
//    assert(value->value.pointer != value->movedTo->value.pointer);
//    assert(strcmp(value->value.pointer, value->movedTo->value.pointer) == 0);
//}
//
//void itMovesImmediateValues() {
//    JsValue* value = allocateJsValue(Number);
//    moveValue(value);
//    assert(value->movedTo != value);
//    assert(isMoved(value));
//    assert(value->movedTo->type == Number);
//}

