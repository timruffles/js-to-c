/**
 * Environments - currently implemented on top of objects.
 */
#include <stdlib.h>
#include <stdio.h>

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

/**
 * Create a env used for a given function call
 **/
Env *envCreateForCall(Env* parent, JsValue* argumentNames[], uint64_t argCount, JsValue* argumentValues[], uint64_t suppliedArgumentCount) {
    Env* callEnv = envCreate(parent);
    log_info("Created call env %p parent %p, now looping over %llu args", callEnv, parent, argCount);
    for(uint64_t i = 0; i < argCount; i++) {
        log_info("Env name %s", stringGetCString(argumentNames[i]));
        envDeclare(callEnv, argumentNames[i]);
        JsValue* val = i < suppliedArgumentCount ? argumentValues[i] : getUndefined();
        envSet(callEnv, argumentNames[i], val);
    }
    log_info("Setup new call env");
    return callEnv;
}

JsValue* envGet(Env *env, JsValue *name) {
    precondition(jsValueType(name) == STRING_TYPE, "Attempted to lookup non-string in env");
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
        log_info("Looked up undeclared variable '%s'", stringGetCString(name));
        exceptionsThrowReferenceError(
            stringCreateFromTemplate("%s is not defined", stringGetCString(name)));
    }
    log_info("Looked up '%s' in %p got type %s", stringGetCString(name), env, jsValueReflect(found).name);
    return found;
}

void envSet(Env *env, JsValue *name, JsValue *value) {
    Env* lookup = env;
    while(lookup) {
        JsValue* found = objectInternalOwnProperty(lookup, name);
        if(found == NULL) {
            lookup = objectEnvGetParent(lookup);
        } else {
            objectSet(lookup, name, value);
            return;
        }
    }

    log_info("Setting undeclared %s", stringGetCString(name));
    exceptionsThrowReferenceError(
        stringCreateFromTemplate("%s is not defined", stringGetCString(name)));
}

void envDeclare(Env *env, JsValue *name) {
    objectSet(env, name, getUndefined());
}

void envDestroy() {
    // currently NOOP
}


