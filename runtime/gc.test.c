#pragma clang diagnostic ignored "-Wmissing-prototypes"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "gc.h"
#include "language.h"
#include "objects.h"
#include "strings.h"
#include "config.h"
#include "runtime.h"

#define JS_SET(O,P,V) objectSet(O,stringCreateFromCString(P),V)
#define JS_GET(O,P) objectGet(O,stringCreateFromCString(P))

typedef struct {
    GcHeader;
    uint64_t number;
} TestStruct;

static void itCanTestInitWithoutInit() {
    _gcTestInit();
}

static void itCanTestInitAfterInit() {
    // this inits twice, freeing first time
    _gcTestInit();
    _gcTestInit();
}

static void itAllocates() {
    _gcTestInit();
    void* value = gcAllocate(sizeof(uint64_t), 1); 
    assert(value != NULL);
}

static void itSetsSizeAndType() {
    _gcTestInit();

    TestStruct* valueA = gcAllocate(sizeof(TestStruct), 1); 

    assert(valueA->size == sizeof(TestStruct));
    assert(valueA->type == 1);
}

static void itTracksSpace() {
    _gcTestInit();

    TestStruct* valueA = gcAllocate(sizeof(TestStruct), 1); 
    GcObject* freeSpace = (void*)((char*)valueA + sizeof(TestStruct));

    assert(valueA->size == sizeof(TestStruct));
    assert(freeSpace->type == FREE_SPACE_TYPE);
    GcStats stats = gcStats();
    assert(freeSpace->size == stats.heapSize - sizeof(TestStruct));
}

static void itGarbageCollectsCorrectly() {
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

    JsValue* roots[] = {root};

    _gcVisualiseHeap();
    _gcRun(roots, 1);
    _gcVisualiseHeap();

    JsValue* newLiveOne = JS_GET(root, "liveOne");
    JsValue* newLiveTwo = JS_GET(root, "liveTwo");

    assert(jsValueNumber(JS_GET(newLiveOne, "id")) == 101);
    assert(jsValueNumber(JS_GET(JS_GET(newLiveTwo, "liveDeepOne"), "id")) == 2002);

    assert(JS_GET(root, "liveOne")
        == JS_GET(root, "liveOneSecondRef"));

    // garbage is zeroed, with a FreeSpace header appended
    assert(jsValueType(garbageOne) == FREE_SPACE_TYPE);
    assert(jsValueType(garbageTwo) == FREE_SPACE_TYPE);
}


int main() {
    configInitFromEnv();

    test(itCanTestInitWithoutInit); 
    test(itCanTestInitAfterInit); 
    test(itAllocates); 
    test(itSetsSizeAndType);
    test(itTracksSpace); 

    test(itGarbageCollectsCorrectly);
}
