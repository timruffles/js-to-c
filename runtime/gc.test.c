#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "gc.h"
#include "language.h"
#include "objects.h"
#include "strings.h"
#include "config.h"

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

void itReusesFreeSpace() {
}

void itGarbageCollectsCorrectly() {
    // note - this will crash nastily if we force a GC before
    // manually running GC - as it'll use runtime env
    _gcTestInit();
    runtimeInit();

    JsValue* liveOne = objectCreatePlain();
    JsValue* garbageOne = objectCreatePlain();
    JsValue* liveTwo = objectCreatePlain();
    JsValue* garbageTwo = objectCreatePlain();
    JsValue* liveDeepOne = objectCreatePlain();

    JsValue* root = objectCreatePlain();
    JS_SET(root, "liveOne", liveOne);
    JS_SET(liveOne, "thingy", jsValueCreateNumber(42));
    JS_SET(liveOne, "id", jsValueCreateNumber(101));
    JS_SET(liveOne, "other", jsValueCreateNumber(42));
    JS_SET(root, "liveTwo", liveTwo);
    JS_SET(root, "liveOneSecondRef", liveOne);

    JS_SET(liveTwo, "liveDeepOne", liveDeepOne);
    JS_SET(liveDeepOne, "id", jsValueCreateNumber(2002));

    GcStats before = gcStats();

    JsValue* roots[] = {root};

    _gcRun(roots, 1);

    JsValue* newLiveOne = JS_GET(root, "liveOne");
    JsValue* newLiveTwo = JS_GET(root, "liveTwo");

    assert(jsValueNumber(JS_GET(newLiveOne, "id")) == 101);
    assert(jsValueNumber(JS_GET(JS_GET(newLiveTwo, "liveDeepOne"), "id")) == 2002);

    assert(JS_GET(root, "liveOne")
        == JS_GET(root, "liveOneSecondRef"));

    // garbage is zeroed
    assert(jsValueType(garbageOne) == 0);
    GcObject* garbageTwoGc = (void*)garbageTwo;
    assert(garbageTwoGc->next == 0);

    GcStats after = gcStats();
    int64_t saved = (int64_t)(before.used - after.used);
    assert(saved > 0);
}


int main() {
    configInitFromEnv();

    test(itCanTestInitWithoutInit); 
    test(itCanTestInitAfterInit); 
    test(itCanAllocate); 
    test(itSetsNextPointerToPositionOfObjectAllocatedNext);

    test(itMovesRoots);
    test(itGarbageCollectsCorrectly);
}
