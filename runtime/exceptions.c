#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#include "language.h"
#include "_memory.h"
#include "gc.h"
#include "objects.h"
#include "strings.h"
#include "exceptions.h"
#include "environments.h"
#include "runtime.h"
#include "lib/debug.h"

jmp_buf exceptionsJumpBuf;

typedef struct JsCatch {
    struct JsCatch* parent;
    bool isRoot;
    Env* env;
    jmp_buf jumpBuf;
    GcProtectedValue* gcProtectedValues;
} JsCatch;

static void unhandledException(JsValue* error) {
    char buffer[8192];

    sprintf(buffer, "Uncaught %s: %s",
      stringGetCString(objectGet(error, stringFromLiteral("name"))),
      stringGetCString(objectGet(error, stringFromLiteral("message"))));

    fprintf(stderr, "%s\n", buffer);
    assert(0);
}

// called by exceptionsTry macro to implement try/catch
void exceptionsCatchStart(Env* env) {
    RuntimeEnvironment* runtime = runtimeGet();
    JsCatch* catch;
    ensureCallocBytes(catch, sizeof(JsCatch));

    // save current jump buf before the macro overwrites it
    if(!runtime->catchStack->isRoot) {
        memcpy(runtime->catchStack->jumpBuf, exceptionsJumpBuf, sizeof(jmp_buf));
    }

    *catch = (JsCatch) {
        .parent = runtime->catchStack,
        .env = env,
    };
    runtime->catchStack = catch;
}

static void catchStackPop() {
    RuntimeEnvironment* runtime = runtimeGet();
    precondition(runtime->catchStack != NULL, "catch stack invalid, should never be NULL");
    precondition(!runtime->catchStack->isRoot, "catch stack should never attempt to pop root");

    JsCatch* newHead = runtime->catchStack->parent;
    free(runtime->catchStack);
    runtime->catchStack = newHead;
    if(!newHead->isRoot) {
        memcpy(exceptionsJumpBuf, newHead->jumpBuf, sizeof(jmp_buf));
    }
}

void exceptionsCatchEnd() {
    RuntimeEnvironment* runtime = runtimeGet();
    if(runtime->catchStack->isRoot) {
        fail("Ending catch when none started");
    }
    catchStackPop();
}

void exceptionsThrow(JsValue* error) {
    RuntimeEnvironment* runtime = runtimeGet();
    if(runtime->catchStack->isRoot) {
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


void _exceptionsGcProtect(JsValue* value) {
    RuntimeEnvironment* rt = runtimeGet();
    GcObject* obj = (void*)value;
    GcProtectedValue* head = rt->catchStack->gcProtectedValues;
    GcProtectedValue* newHead;
    ensureCallocBytes(newHead, sizeof(GcProtectedValue));
    *newHead = (GcProtectedValue){
        .next = head,
        .value = obj,
    };
    rt->catchStack->gcProtectedValues = newHead;
}

void _exceptionsGcUnprotectAfterThrow() {
    RuntimeEnvironment* rt = runtimeGet();
    JsCatch* stack = rt->catchStack;
    while(stack->gcProtectedValues != NULL) {
        _exceptionsGcUnprotect();
    }
}

void _exceptionsGcUnprotect() {
    RuntimeEnvironment* rt = runtimeGet();
    JsCatch* stack = rt->catchStack;
    precondition(stack->gcProtectedValues != NULL, "popped too many values");
    GcObject* value = stack->gcProtectedValues->value;
    value->marked = false;
    stack->gcProtectedValues = stack->gcProtectedValues->next;
}

JsCatch* exceptionsRootCatchCreate() {
    JsCatch* catch;
    ensureCallocBytes(catch, sizeof(JsCatch));
    *catch = (JsCatch) {
        .parent = NULL,
        .isRoot = true,
    };
    return catch;
}
void _exceptionsGcForeachValue(GcCallback* callback) {
    RuntimeEnvironment* rt = runtimeGet();
    JsCatch* stack = rt->catchStack;
    while(stack) {
        log_info("Traversing catch frame");
        GcProtectedValue* node = stack->gcProtectedValues;
        while(node != NULL) { 
            log_info("Traversed value on catch frame");
            callback(node->value);
            node = node->next;
        }
        stack = stack->parent;
    }
}

