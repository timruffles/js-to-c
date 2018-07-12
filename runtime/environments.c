#include <stdlib.h>
#include <stdio.h>

#include "./lib/uthash.h"

#include "environments.h"
#include "language.h"
#include "exceptions.h"

static void *const ROOT_HAS_NO_PARENT = 0;

/**
 * All of our environment identifiers are known
 * at compile time (there's no way to dynamically
 * assign a variable name), we preallocate all identifier strings and
 * use pointer comparison for equality.
 */
typedef struct {
    JsIdentifier *id;
    JsValue *value;

    UT_hash_handle hh;
} EnvRecord;


typedef struct Env {
    EnvRecord *properties;
    struct Env *const parent;
} Env;

Env *envCreateRoot() {
    return envCreate(ROOT_HAS_NO_PARENT);
}

Env *envCreate(Env *const parent) {
    Env *env = calloc(sizeof(Env), 1);
    *env = (Env) {
            .properties = NULL,
            .parent = parent
    };
    return env;
}

JsValue *envGet(Env *env, JsIdentifier *name) {
    EnvRecord *record;
    HASH_FIND_PTR(env->properties, &name, record);
    return record == NULL
           ? getUndefined()
           : record->value;
}

void envSet(Env *env, JsIdentifier *name, JsValue *value) {
    EnvRecord *record;
    HASH_FIND_PTR(env->properties, &name, record);
    if (record == NULL) {
        throwError("Attempted to set undeclared variable");
    }
    record->value = value;
}

void envDeclare(Env *env, JsIdentifier *name) {
    EnvRecord *r = calloc(sizeof(EnvRecord), 1);
    *r = (EnvRecord) {
            .id = name,
            .value = getUndefined(),
    };
    HASH_ADD_PTR(env->properties, id, r);
}

void envDestroy(Env *env) {
    free(env);
}


