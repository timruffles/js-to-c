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

static void itCanCreateRootEnv() {
    Env* env = envCreateRoot();
    assert(env != NULL);
}

static void itCanDestroyAnEnv() {
    Env* env = envCreateRoot();
    assert(env != NULL);
    envDestroy(env);
}

static void itCanDeclareAVariable() {
    Env* env = envCreateRoot();
    envDeclare(env, stringFromLiteral("one"));

    envDestroy(env);
}

static void itReturnsUndefinedForDeclareButUndefined() {
    Env* env = envCreateRoot();
    envDeclare(env, stringFromLiteral("one"));
    JsValue* val = envGet(env, stringFromLiteral("one"));

    assert(val == getUndefined());
    envDestroy(env);
}

static void itCanAssignAVariable() {
    Env* env = envCreateRoot();
    envDeclare(env, stringFromLiteral("one"));
    envSet(env, stringFromLiteral("one"), getTrue());

    envDestroy(env);
}

static void itCanGetAVariableValue() {
    Env* env = envCreateRoot();
    envDeclare(env, stringFromLiteral("one"));
    envSet(env, stringFromLiteral("one"), getTrue());

    assert(envGet(env, stringFromLiteral("one")) == getTrue());
    assert(envGet(env, stringFromLiteral("one")) != getFalse());

    envDestroy(env);
}

static void itUsesStringValueNotIdentifyForGet() {
    Env* env = envCreateRoot();

    envDeclare(env, stringFromLiteral("x"));
    envSet(env, stringFromLiteral("x"), getTrue());

    assert(
        envGet(env, stringFromLiteral("x")) 
        == envGet(env, stringFromLiteral("x"))
    );
}

static void itUsesStringValueNotIdentifyForSet() {
    Env* env = envCreateRoot();

    envDeclare(env, stringFromLiteral("x"));
    envSet(env, stringFromLiteral("x"), getTrue());
    envSet(env, stringFromLiteral("x"), getFalse());

    assert(
        envGet(env, stringFromLiteral("x")) == getFalse()
    );
}

static void itSetsUpArgumentValuesInCallEnv() {
    Env* env = envCreateRoot();
    JsValue* names[] = {stringFromLiteral("one"), stringFromLiteral("two")};
    JsValue* values[] = {getTrue(), stringFromLiteral("twoValue")};
    Env* callEnv = envCreateForCall(env, names, values, 2);

    assert(callEnv != NULL);

    assert(envGet(callEnv, stringFromLiteral("one")) == getTrue());
    assert(stringIsEqual(envGet(callEnv, stringFromLiteral("two")), stringFromLiteral("twoValue")));
}

static void itAffectsNearestAncestorWithDeclaredVar() {
    Env* env = envCreateRoot();
    JsValue* varA = stringFromLiteral("one");

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

int main() {
    testLanguageAndGcInit();

    test(itCanCreateRootEnv);
    test(itCanDestroyAnEnv);
    test(itCanDeclareAVariable);
    test(itReturnsUndefinedForDeclareButUndefined);
    test(itCanAssignAVariable);
    test(itUsesStringValueNotIdentifyForGet);
    test(itUsesStringValueNotIdentifyForSet);
    test(itCanGetAVariableValue);

    test(itSetsUpArgumentValuesInCallEnv);
    test(itAffectsNearestAncestorWithDeclaredVar);
}

