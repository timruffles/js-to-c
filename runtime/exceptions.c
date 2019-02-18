#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#include "language.h"
#include "objects.h"
#include "strings.h"
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
    char buffer[8192];

    sprintf(buffer, "Uncaught %s: %s",
      stringGetCString(objectGet(error, stringFromLiteral("name"))),
      stringGetCString(objectGet(error, stringFromLiteral("message"))));

    fprintf(stderr, "%s", buffer);
    assert(0);
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

void exceptionsThrow(JsValue* error) {
    RuntimeEnvironment* runtime = runtimeGet();
    if(runtime->catchStack == NULL) {
        unhandledException(error);
    } else {
        runtime->thrownError = error;

        jmp_buf target;
        memcpy(target, exceptionsJumpBuf, sizeof(jmp_buf));

        catchStackPop();
        longjmp(target, 1);
    }
}

static JsValue* errorCreate(JsValue* name, JsValue* message) {
    JsValue* obj = objectCreatePlain();
    JS_SET(obj, "name", name);
    JS_SET(obj, "message", message);
    return obj;
}

void exceptionsThrowReferenceError(JsValue* message) {
    exceptionsThrow(errorCreate(stringFromLiteral("ReferenceError"), message));
}

void exceptionsThrowTypeError(JsValue* message) {
    exceptionsThrow(errorCreate(stringFromLiteral("TypeError"), message));
}


