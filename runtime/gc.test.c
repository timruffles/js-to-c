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
#include "_freelist.h"

typedef struct {
    GcHeader;
    uint64_t number;
} TestStruct;

static void itCanTestInitWithoutInit() {
    _gcTestInit(NULL);
}

static void itCanTestInitAfterInit() {
    // this inits twice, freeing first time
    _gcTestInit(NULL);
    _gcTestInit(NULL);
}

static void itAllocates() {
    _gcTestInit(NULL);
    void* value = gcAllocate(sizeof(uint64_t), 1); 
    assert(value != NULL);
}

static void itSetsSizeAndType() {
    _gcTestInit(NULL);

    TestStruct* valueA = gcAllocate(sizeof(TestStruct), 1); 

    assert(valueA->size == sizeof(TestStruct));
    assert(valueA->type == 1);
}

static void itGarbageCollectsCorrectly() {
    _gcTestInit(NULL);

    JsValue* liveOne = objectCreatePlain();
    JsValue* garbageOne = objectCreatePlain();
    JsValue* liveTwo = objectCreatePlain();
    JsValue* garbageTwo = objectCreatePlain();
    JsValue* liveDeepOne = objectCreatePlain();

    JsValue* root = runtimeGet()->global;
    JS_SET(root, "liveOne", liveOne);
    JS_SET(liveOne, "thingy", jsValueCreateNumber(42));
    JS_SET(liveOne, "id", jsValueCreateNumber(101));
    JS_SET(liveOne, "other", jsValueCreateNumber(42));
    JS_SET(root, "liveTwo", liveTwo);
    JS_SET(root, "liveOneSecondRef", liveOne);

    JS_SET(liveTwo, "liveDeepOne", liveDeepOne);
    JS_SET(liveDeepOne, "id", jsValueCreateNumber(2002));

    _gcVisualiseHeap(NULL);
    _gcRunGlobal();
    _gcVisualiseHeap(NULL);
    
    // check garbage is zeroed, with a FreeSpace header appended
    log_info("%s", gcObjectReflectType(jsValueType(garbageOne)).name);
    assert(jsValueType(garbageOne) == FREE_SPACE_TYPE);
    assert(jsValueType(garbageTwo) == FREE_SPACE_TYPE);

    JsValue* newLiveOne = JS_GET(root, "liveOne");
    JsValue* newLiveTwo = JS_GET(root, "liveTwo");

    assert(jsValueNumber(JS_GET(newLiveOne, "id")) == 101);
    assert(jsValueNumber(JS_GET(JS_GET(newLiveTwo, "liveDeepOne"), "id")) == 2002);

    assert(JS_GET(root, "liveOne")
        == JS_GET(root, "liveOneSecondRef"));

    assert(JS_GET(root, "liveOne") == liveOne);

    JsValue* newOne = objectCreatePlain();
    JsValue* newTwo = objectCreatePlain();
    assert(newOne != newTwo);
}

static void itCanGcObjectProperties() {
    _gcTestInit(NULL);

    JsValue* garbageOne = objectCreatePlain();
    JS_SET(garbageOne, "someProp", jsValueCreateNumber(10));

    _gcRunGlobal();
    // TODO would be nice to test it's freed
}

static void itCanPreventGcInTheMiddleOfAGroupOfOperations() {
    // We often need to do a set of operations atomically - e.g
    // setup a call environment. If GC occured midway we could end
    // up with an inconsistent state
    _gcTestInit(NULL);

    JsValue* notInGroup = objectCreatePlain();

    // start a group - although there is no path from root
    // to these objects they should stay live
    GcAtomicId id = gcAtomicGroupStart();
    JsValue* itemOne = objectCreatePlain();
    JS_SET(itemOne, "someProp", jsValueCreateNumber(10.3));
    JsValue* itemTwo = objectCreatePlain();

    _gcRunGlobal();
    JS_SET(itemTwo, "itemOne", itemOne);
    gcAtomicGroupEnd(id);

    assert(jsValueNumber(JS_GET(JS_GET(itemTwo, "itemOne"), "someProp")) == 10.3);
    assert(jsValueType(notInGroup) == FREE_SPACE_TYPE);
}

static void itPreventsOverAllocation() {
    Config config = (Config){
      .heapSize = 4000
    };

    _gcTestInit(&config);

    _gcVisualiseHeap(NULL);
    void* allocated = _gcAllocate(10000, STRING_TYPE);
    log_info("%p", allocated);
    assert(allocated == NULL);
}

static void itCanReuseMemory() {
    Config config = (Config){
      .heapSize = 1200
    };
    _gcTestInit(&config);
    log_info("Step: after init:");
    JsValue* root = runtimeGet()->global;
    _gcVisualiseHeap(&(GcVisualiseHeapOpts){
      .highlight = root,
    });

    for(int i = 0; i < 8; i++) {
        objectCreatePlain();
    }

    log_info("Step: allocated 10 objects of garbage");
    _gcVisualiseHeap(NULL);

    log_info("Step: allocating live object");
    JsValue* liveOne = objectCreatePlain();

    GcAtomicId gid = gcAtomicGroupStart();
    JsValue* strAlloced = stringFromLiteral("liveOne");
    objectSet(root, strAlloced, liveOne);
    gcAtomicGroupEnd(gid);
    log_info("Alloced string at %p", strAlloced);
    //JS_SET(root, "liveOne", liveOne);
    
    _gcVisualiseHeap(&(GcVisualiseHeapOpts){
      .highlight = (void*)strAlloced,
    });

    log_info("Step: force GC");
    _gcRunGlobal();

    for(int i = 0; i < 10; i++) {
        objectCreatePlain();
    }

    log_info("Step: after GC, allocated 10 objects of garbage");
    _gcVisualiseHeap(&(GcVisualiseHeapOpts){
      .highlight = (void*)liveOne,
    });

    DEBUG_JS_VAL(liveOne);
    DEBUG_JS_VAL(JS_GET(root, "liveOne"));
    // we got back undefined!
    assert(JS_GET(root, "liveOne") == liveOne);
}


int main() {
    // test(itCanTestInitWithoutInit); 
    // test(itCanTestInitAfterInit); 
    // test(itAllocates); 
    // test(itSetsSizeAndType);

    // test(itGarbageCollectsCorrectly);
    // test(itCanGcObjectProperties);
    // test(itCanPreventGcInTheMiddleOfAGroupOfOperations);
    //test(itPreventsOverAllocation);
    test(itCanReuseMemory);
}
