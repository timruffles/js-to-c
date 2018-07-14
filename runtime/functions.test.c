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
    envGet(env, stringCreateFromCString("x"));
    return getTrue();
}

void itCanCreateAFunctionRecord() {
    FunctionRecord* record = functionRecordCreate(exampleUserFunction);
    assert(record != NULL);
}

void itCanCreateAFunction() {
    FunctionRecord* record = functionRecordCreate(exampleUserFunction);
    JsValue* fn = functionCreate(record);
    assert(fn != NULL);
}

void itCanCallAFunction() {
    FunctionRecord* record = functionRecordCreate(exampleUserFunction);
    JsValue* fn = functionCreate(record);

    Env* env = envCreateRoot();
    JsValue* name = stringCreateFromCString("x");
    envDeclare(env, name);
    envSet(env, name, getTrue());

    JsValue* returned = functionRun(fn, env);
    assert(returned == getTrue());
}

int main() {
    test(itCanCreateAFunctionRecord);
    test(itCanCreateAFunction);
    test(itCanCallAFunction);
}

