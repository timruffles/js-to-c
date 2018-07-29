#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "environments.h"
#include "language.h"
#include "functions.h"
#include "strings.h"
#include "test.h"

// e.g const x => { x; return true }
JsValue* exampleUserFunction(Env* env) {
    assert(envGet(env, stringCreateFromCString("two")) == getTrue());
    assert(jsValueNumber(envGet(env, stringCreateFromCString("one"))) - 7 < 0.000001);
    return getTrue();
}

JsValue** argumentNames;
JsValue** argumentValues;

void itCanCreateAFunction() {
    JsValue* fn = functionCreate(exampleUserFunction, argumentNames, 2);
    assert(fn != NULL);
}

void itCanCallAFunction() {
    JsValue* fn = functionCreate(exampleUserFunction, argumentNames, 2);
    assert(fn != NULL);

    Env* env = envCreateRoot();
    Env* callEnv = envCreateForCall(env, argumentNames, argumentValues, 2);

    JsValue* returned = functionRun(fn, callEnv);
    assert(returned == getTrue());
}

int main() {
    argumentNames = calloc(2, sizeof(JsValue*));
    argumentValues = calloc(2, sizeof(JsValue*));
    argumentNames[0] = stringCreateFromCString("one");
    argumentNames[1] = stringCreateFromCString("two");
    argumentValues[0] = jsValueCreateNumber(7);
    argumentValues[1] = getTrue();

    test(itCanCreateAFunction);
    test(itCanCallAFunction);
}

