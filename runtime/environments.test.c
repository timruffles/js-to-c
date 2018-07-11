#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "environments.h"
#include "language.h"

#define test(T) (T)(); printf("%s passed\n", #T);

_identifierCreate(idOne);

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

int main(int argc, char** argv) {
    test(itCanCreateRootEnv);
    test(itCanDestroyAnEnv);
    test(itCanDeclareAVariable);
    test(itReturnsUndefinedForDeclareButUndefined);
    test(itCanAssignAVariable);
}

