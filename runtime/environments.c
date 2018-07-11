#include "environments.h"
#include <stdlib.h>

static void* const ROOT_HAS_NO_PARENT = 0;

Env* EnvCreateRoot() {
    return EnvCreate(ROOT_HAS_NO_PARENT);
}

Env* EnvCreate(Env* const parent) {
    Env* obj = calloc(sizeof(Env), 1);
    Env e = {
            .properties = NULL,
            .parent = parent
    };
    *obj = e;
    return obj;
}

void EnvDestroy(Env* env) {
    free(env);
}
