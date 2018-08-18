#include <stdlib.h>
#include <assert.h>

#include "lib/debug.h"
#include "global.h"
#include "runtime.h"
#include "config.h"
#include "environments.h"
#include "language.h"
#include "gc.h"

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
    assert(!"Issue - the gc roots array needs to be correctly sized for the elements, which isn't sizeof(GcObject), it's sizeof(JsValue). Can't iterate with incorrect size. Could use a linked list instead?");
    gcRoots[0] = (GcObject*)globalEnv;

    *runtime = (RuntimeEnvironment) {
        .gcRoots = gcRoots,
        .gcRootsCount = gcRootsCount,
    };

    return runtime;
}

RuntimeEnvironment* runtimeInit() {
    configInitFromEnv();
    log_info("read config");

    gcInit();
    log_info("setup gc");

    log_info("created runtime environment");
    runtimeEnv = runtimeCreate();
    return runtimeEnv;
}

