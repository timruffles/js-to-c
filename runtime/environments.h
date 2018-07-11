#pragma once

#include "language.h"

typedef struct Env {
   void *properties;
   struct Env* const parent;
 } Env;

/**
 * Creates a new environment with the specified parent
 */
Env *EnvCreateRoot();
Env *EnvCreate(Env *const parent);
void EnvDestroy(Env*);

Env *EnvDeclare(Env, JsString name);
Env *EnvSet(Env, JsIdentifier, JsValue value);
Env *EnvGet(Env, JsIdentifier);

