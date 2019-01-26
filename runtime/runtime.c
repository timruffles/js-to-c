#include <stdlib.h>
#include <assert.h>

#include "lib/debug.h"
#include "global.h"
#include "runtime.h"
#include "config.h"
#include "environments.h"
#include "language.h"
#include "gc.h"
#include "gcObject.h"

static RuntimeEnvironment* runtimeEnv;

RuntimeEnvironment* runtimeGet() {
    assert(runtimeEnv != NULL);
    return runtimeEnv;
}

static RuntimeEnvironment* runtimeCreate() {
    RuntimeEnvironment* runtime = calloc(1, sizeof(RuntimeEnvironment));
    ensureAlloced(runtime);

    *runtime = (RuntimeEnvironment) {
        .gcAtomicGroupId = 1,
        .gcAtomicGroupDepth = 0,
    };

    return runtime;
}

RuntimeEnvironment* runtimeInit(Config* maybeConfig) {
    runtimeEnv = runtimeCreate();

    Config* config = maybeConfig;
    if(config == NULL) {
        // leaks, but no biggy as it's a one time outside tests
        // TODO just embed config in runtime
        config = calloc(1, sizeof(Config));
    }
    ensureAlloced(config);
    configInit(config);

    log_info("read config");

    gcInit(config);
    log_info("setup gc");

    uint64_t gcRootsCount = 1;
    JsValue** gcRoots = calloc(gcRootsCount, sizeof(JsValue*));
    JsValue* global = createGlobalObject();
    // TODO use globalEnv created in createGlobalObject() - currently it's a noop to convert between but won't always be true
    Env* globalEnv = envFromGlobal(global);
    gcRoots[0] = (JsValue*)globalEnv;

    runtimeEnv->gcRoots = gcRoots;
    runtimeEnv->gcRootsCount = gcRootsCount;
    runtimeEnv->globalEnv = globalEnv;
    runtimeEnv->global = global;

    runtimeEnv->config = config;
    log_info("created runtime environment");
    return runtimeEnv;
}


void runtimeEnterEnv(Env* env) {
    RuntimeEnvironment* rt = runtimeGet();
    precondition(rt->stackDepth < RUNTIME_MAX_STACK_DEPTH, "hit max stack frame count %i", RUNTIME_MAX_STACK_DEPTH);
    rt->callStack[rt->stackDepth] = (RuntimeStackPointer) {
        .env = env,
    };
    rt->stackDepth += 1;
}

void runtimeExitEnv() {
    RuntimeEnvironment* rt = runtimeGet();
    precondition(rt->stackDepth > 0, "attempted to exit env when none present");
    rt->stackDepth -= 1;
}

void runtimeGcTraverse(GcCallback* cb) {
    RuntimeEnvironment* rt = runtimeGet();
    log_info("GC call env stack %i", rt->stackDepth);
    for(int depth = rt->stackDepth;
        depth > 0;
        depth--) {
        log_info("GC'd call env");
        cb(rt->callStack[depth - 1].env);
    }
}
