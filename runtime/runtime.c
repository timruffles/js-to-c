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

    uint64_t gcRootsCount = 1;
    JsValue** gcRoots = calloc(gcRootsCount, sizeof(JsValue*));
    JsValue* global = createGlobalObject();
    Env* globalEnv = envFromGlobal(global);
    gcRoots[0] = (JsValue*)globalEnv;

    *runtime = (RuntimeEnvironment) {
        .gcRoots = gcRoots,
        .gcRootsCount = gcRootsCount,
        .globalEnv = globalEnv,
        .global = global,
        .gcAtomicGroupId = 0,
    };

    return runtime;
}

RuntimeEnvironment* runtimeInit() {
    configInitFromEnv();
    log_info("read config");

    gcInit();
    log_info("setup gc");

    runtimeEnv = runtimeCreate();
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
