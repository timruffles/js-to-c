#include <stdlib.h>
#include <stdio.h>

#include "./lib/uthash.h"

#include "environments.h"
#include "objects.h"
#include "language.h"
#include "exceptions.h"
#include "strings.h"

/**
 * Currently envs are just objects, so this works fine.
 */
Env *envFromGlobal(JsValue* globalObject) {
    return globalObject;
}

Env *envCreateRoot() {
    return objectCreatePlain();
}

Env *envCreate(Env* parent) {
    return objectCreate(parent);
}

Env *envCreateForCall(Env* parent, JsValue* argumentNames[], JsValue* argumentValues[], uint64_t argCount) {
    Env* callEnv = objectCreate(parent);
    for(uint64_t i = 0; i < argCount; i++) {
        envDeclare(callEnv, argumentNames[i]);
        envSet(callEnv, argumentNames[i], argumentValues[i]);
    }
    return callEnv;
}

JsValue *envGet(Env *env, JsValue *name) {
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
        printf("Looked up undeclared %s\n", stringGetCString(name));
        throwError("Attempted to lookup undeclared variable");
    }
    return found;
}

void envSet(Env *env, JsValue *name, JsValue *value) {
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
        printf("Tried to assign undeclared %s\n", stringGetCString(name));
        throwError("Attempted to assign undeclared variable");
    }
    objectSet(env, name, value);
}

void envDeclare(Env *env, JsValue *name) {
    objectSet(env, name, getUndefined());
}

void envDestroy(Env *env) {
    free(env);
}


