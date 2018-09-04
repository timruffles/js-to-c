#pragma once
#include <stdint.h>

#include "language.h"
#include "environments.h"
#include "exceptions.h"

typedef struct RuntimeEnvironment {
    JsValue** gcRoots;
    uint64_t gcRootsCount;

    Env* globalEnv;

    JsCatch* catchStack;

    JsValue* thrownError;
} RuntimeEnvironment;

RuntimeEnvironment* runtimeInit(void);
RuntimeEnvironment* runtimeGet(void);
