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
    gcRoots[0] = (JsValue*)globalEnv;

    *runtime = (RuntimeEnvironment) {
        .gcRoots = gcRoots,
        .gcRootsCount = gcRootsCount,
        .globalEnv = globalEnv,
    };

    return runtime;
}

RuntimeEnvironment* runtimeInit() {
    configInitFromEnv();
    log_info("read config");


    gcInit();
    log_info("setup gc");

    languageInit();
    log_info("initialised language");

    runtimeEnv = runtimeCreate();
    log_info("created runtime environment");
    return runtimeEnv;
}

