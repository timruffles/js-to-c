#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "environments.h"
#include "language.h"
#include "functions.h"
#include "strings.h"
#include "test.h"
#include "gc.h"

// e.g const x => { x; return true }
JsValue* exampleUserFunction(Env* env) {
    assert(envGet(env, stringFromLiteral("two")) == getTrue());
    assert(jsValueNumber(envGet(env, stringFromLiteral("one"))) - 7 < 0.000001);
    return getTrue();
}

static JsValue** argumentNames;
static JsValue** argumentValues;
static Env* exampleEnv;

static void itCanCreateAFunction() {
    JsValue* fn = functionCreate(exampleUserFunction, argumentNames, 2, exampleEnv);
    assert(fn != NULL);
}

static void itCanCallAFunction() {
    JsValue* fn = functionCreate(exampleUserFunction, argumentNames, 2, exampleEnv);
    assert(fn != NULL);

    Env* callEnv = envCreateForCall(exampleEnv, argumentNames, argumentValues, 2);

    JsValue* returned = _functionRun(fn, callEnv);
    assert(returned == getTrue());
}

int main() {
    testLanguageAndGcInit();

    argumentNames = calloc(2, sizeof(JsValue*));
    argumentValues = calloc(2, sizeof(JsValue*));
    argumentNames[0] = stringFromLiteral("one");
    argumentNames[1] = stringFromLiteral("two");
    argumentValues[0] = jsValueCreateNumber(7);
    argumentValues[1] = getTrue();

    test(itCanCreateAFunction);
    test(itCanCallAFunction);
}

