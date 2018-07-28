#include <stdlib.h>
#include <stdio.h>

#include "./lib/uthash.h"

#include "environments.h"
#include "objects.h"
#include "language.h"
#include "exceptions.h"
#include "strings.h"
#include "lib/debug.h"

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
    Env* callEnv = envCreate(parent);
    log_info("Created cal env, now looping over %i args", argCount);
    for(uint64_t i = 0; i < argCount; i++) {
        log_info("Env name %p", argumentNames[0]);
        envDeclare(callEnv, argumentNames[i]);
        envSet(callEnv, argumentNames[i], argumentValues[i]);
    }
    log_info("Setup new call env");
    return callEnv;
}

JsValue *envGet(Env *env, JsValue *name) {
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
        log_info("Looked up undeclared %s", stringGetCString(name));
        throwError("Attempted to lookup undeclared variable");
    }
    log_info("Looked up %s got type %s", stringGetCString(name), jsValueType(found));
    return found;
}

void envSet(Env *env, JsValue *name, JsValue *value) {
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
        log_info("Setting undeclared %s", stringGetCString(name));
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


