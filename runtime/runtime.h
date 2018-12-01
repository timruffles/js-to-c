#pragma once
#include <stdint.h>

#include "language.h"
#include "environments.h"
#include "exceptions.h"
#include "gc.h"

#define RUNTIME_MAX_STACK_DEPTH 256

#define RUNTIME_GC_ATOMIC_GROUP_MAX 64

typedef struct RuntimeStackPointer {
    Env* env;
} RuntimeStackPointer;


typedef struct RuntimeEnvironment {
    JsValue** gcRoots;
    uint64_t gcRootsCount;

    Env* globalEnv;
    JsValue* global;

    JsCatch* catchStack;

    JsValue* thrownError;

    RuntimeStackPointer callStack[RUNTIME_MAX_STACK_DEPTH];
    int stackDepth;

    int gcAtomicGroupId;

} RuntimeEnvironment;



RuntimeEnvironment* runtimeInit(void);
RuntimeEnvironment* runtimeGet(void);

// what's the current function call env
void runtimeEnterEnv(Env*);
void runtimeExitEnv(void);
void runtimeGcTraverse(GcCallback* cb);
