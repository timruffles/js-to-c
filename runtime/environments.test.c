#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "environments.h"
#include "language.h"
#include "test.h"
#include "strings.h"
#include "operators.h"
#include "gc.h"

#define DECLARE_AND_SET(E,K,V) envDeclare(E,K); envSet(E,K,V)
#define STRICT_EQUAL(A,B) strictEqualOperator(A,B) == getTrue()

JsValue* idOne;

void itCanCreateRootEnv() {
    Env* env = envCreateRoot();
    assert(env != NULL);
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

    assert(val == getUndefined());
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

void itSetsUpArgumentValuesInCallEnv() {
    Env* env = envCreateRoot();
    JsValue* names[] = {stringCreateFromCString("one"), stringCreateFromCString("two")};
    JsValue* values[] = {getTrue(), stringCreateFromCString("twoValue")};
    Env* callEnv = envCreateForCall(env, names, values, 2);

    assert(callEnv != NULL);

    assert(envGet(callEnv, stringCreateFromCString("one")) == getTrue());
    assert(stringComparison(envGet(callEnv, stringCreateFromCString("two")), stringCreateFromCString("twoValue")) == 0);
}

void itAffectsNearestAncestorWithDeclaredVar() {
    Env* env = envCreateRoot();
    JsValue* varA = stringCreateFromCString("one");

    DECLARE_AND_SET(env, varA, jsValueCreateNumber(0));
    Env* childA = envCreate(env);
    Env* childB = envCreate(childA);

    envSet(childB, varA, jsValueCreateNumber(5));

    assert(STRICT_EQUAL(jsValueCreateNumber(5),
                envGet(env, varA)));
    assert(STRICT_EQUAL(jsValueCreateNumber(5),
                envGet(childA, varA)));
    assert(STRICT_EQUAL(jsValueCreateNumber(5),
                envGet(childB, varA)));
}

int main(int argc, char** argv) {
    _gcTestInit();

    idOne = stringCreateFromCString("one");

    test(itCanCreateRootEnv);
    test(itCanDestroyAnEnv);
    test(itCanDeclareAVariable);
    test(itReturnsUndefinedForDeclareButUndefined);
    test(itCanAssignAVariable);
    test(itUsesStringValueNotIdentifyForGet);
    test(itUsesStringValueNotIdentifyForSet);

    test(itSetsUpArgumentValuesInCallEnv);
    test(itAffectsNearestAncestorWithDeclaredVar);
}

