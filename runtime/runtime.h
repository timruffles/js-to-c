#pragma once
#include <stdint.h>

#include "language.h"
#include "environments.h"

typedef struct RuntimeEnvironment {
    JsValue** gcRoots;
    uint64_t gcRootsCount;

    Env* globalEnv;
} RuntimeEnvironment;

RuntimeEnvironment* runtimeInit(void);
RuntimeEnvironment* runtimeGet(void);
