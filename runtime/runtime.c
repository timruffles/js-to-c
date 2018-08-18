#include <stdlib.h>
#include <assert.h>

#include "global.h"
#include "runtime.h"

static RuntimeEnvironment* runtimeEnv;

RuntimeEnvironment* runtimeGet() {
    assert(runtimeEnv != NULL);
    return runtimeEnv;
}

static RuntimeEnvironment* runtimeCreate() {
    RuntimeEnvironment* runtime = calloc(1, sizeof(RuntimeEnvironment));

    uint64_t gcRootsCount = 1;
    JsValue** gcRoots = calloc(gcRootsCount, sizeof(JsValue*));
    JsValue* global = createGlobalObject();
    Env* globalEnv = envFromGlobal(global);
    gcRoots[0] = globalEnv;

    *runtime = (RuntimeEnvironment) {
        .gcRoots = gcRoots,
        .gcRootsCount = gcRootsCount,
    };

    return runtime;
}

RuntimeEnvironment* runtimeInit() {
    configInitFromEnv();
    runtimeEnv = runtimeCreate();
    gcInit();
    return runtimeEnv;
}

