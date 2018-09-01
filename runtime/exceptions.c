#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "language.h"
#include "exceptions.h"
#include "environments.h"
#include "runtime.h"
#include "lib/debug.h"

jmp_buf exceptionsJumpBuf;

typedef struct JsCatch {
    struct JsCatch* parent;
    Env* env;
    jmp_buf jumpBuf;
} JsCatch;

static void unhandledException(JsValue* error) {
    fail("Unhandled exception!");
}

void exceptionsTryStart(Env* env) {
    exceptionsCatchStart(env);
}

void exceptionsCatchStart(Env* env) {
    RuntimeEnvironment* runtime = runtimeGet();
    JsCatch* catch = calloc(1, sizeof(JsCatch));
    ensureAlloced(catch);
    *catch = (JsCatch) {
        .parent = runtime->catchStack,
        .env = env,
    };
    runtime->catchStack = catch;
}

static void catchStackPop() {
    RuntimeEnvironment* runtime = runtimeGet();
    JsCatch* newHead = runtime->catchStack->parent;
    free(runtime->catchStack);
    runtime->catchStack = newHead;
    if(newHead) {
        memcpy(exceptionsJumpBuf, newHead->jumpBuf, sizeof(jmp_buf));
    }
}

void exceptionsCatchEnd() {
    RuntimeEnvironment* runtime = runtimeGet();
    if(runtime->catchStack == NULL) {
        fail("Ending catch when none started");
    }
    catchStackPop();
}

void exceptionsThrow(Env* callEnv, JsValue* error) {
    RuntimeEnvironment* runtime = runtimeGet();
    if(runtime->catchStack == NULL) {
        unhandledException(error);
    } else {
        jmp_buf target;
        memcpy(target, exceptionsJumpBuf, sizeof(jmp_buf));
        catchStackPop();
        longjmp(target, 1);
    }
}

void exceptionsThrowReferenceError(Env* env, JsValue* error) {
    // TODO
}


