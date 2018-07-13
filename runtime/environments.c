#include <stdlib.h>
#include <stdio.h>

#include "./lib/uthash.h"

#include "environments.h"
#include "objects.h"
#include "language.h"
#include "exceptions.h"

Env *envCreateRoot() {
    return objectCreatePlain();
}

Env *envCreate(Env* parent) {
    return objectCreate(parent);
}

JsValue *envGet(Env *env, JsValue *name) {
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
        throwError("Attempted to lookup undeclared variable");
    }
    return found;
}

void envSet(Env *env, JsValue *name, JsValue *value) {
    JsValue* found = objectLookup(env, name);
    if(found == NULL) {
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


