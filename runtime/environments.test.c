#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "environments.h"
#include "language.h"
#include "test.h"
#include "strings.h"

JsValue* idOne;

void itCanCreateRootEnv() {
    Env* env = envCreateRoot();
    assert(env != NULL);
    free(env);
}

void itCanDestroyAnEnv() {
    Env* env = envCreateRoot();
    assert(env != NULL);
    envDestroy(env);
}

void itCanDeclareAVariable() {
    Env* env = envCreateRoot();
    envDeclare(env, idOne);

    envDestroy(env);
}

void itReturnsUndefinedForDeclareButUndefined() {
    Env* env = envCreateRoot();
    envDeclare(env, idOne);
    JsValue* val = envGet(env, idOne);

    envDestroy(env);
}

void itCanAssignAVariable() {
    Env* env = envCreateRoot();
    envDeclare(env, idOne);
    envSet(env, idOne, getTrue());

    envDestroy(env);
}

void itCanGetAVariableValue() {
    Env* env = envCreateRoot();
    envDeclare(env, idOne);
    envSet(env, idOne, getTrue());

    assert(envGet(env, idOne) == getTrue());
    assert(envGet(env, idOne) != getFalse());

    envDestroy(env);
}

void itUsesStringValueNotIdentifyForGet() {
    Env* env = envCreateRoot();

    envDeclare(env, stringCreateFromCString("x"));
    envSet(env, stringCreateFromCString("x"), getTrue());

    assert(
        envGet(env, stringCreateFromCString("x")) 
        == envGet(env, stringCreateFromCString("x"))
    );
}

void itUsesStringValueNotIdentifyForSet() {
    Env* env = envCreateRoot();

    envDeclare(env, stringCreateFromCString("x"));
    envSet(env, stringCreateFromCString("x"), getTrue());
    envSet(env, stringCreateFromCString("x"), getFalse());

    assert(
        envGet(env, stringCreateFromCString("x")) == getFalse()
    );
}

int main(int argc, char** argv) {
    idOne = stringCreateFromCString("one");

    test(itCanCreateRootEnv);
    test(itCanDestroyAnEnv);
    test(itCanDeclareAVariable);
    test(itReturnsUndefinedForDeclareButUndefined);
    test(itCanAssignAVariable);
    test(itUsesStringValueNotIdentifyForGet);
    test(itUsesStringValueNotIdentifyForSet);
}

