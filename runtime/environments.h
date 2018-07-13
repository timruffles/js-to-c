#pragma once

#include "language.h"

typedef JsValue Env;

/**
 * Creates a new environment with the specified parent
 */
Env *envCreateRoot();

Env *envCreate(Env *const parent);

void envDestroy(Env *);

void envDeclare(Env *, JsValue *name);

void envSet(Env *, JsValue *, JsValue *value);

JsValue *envGet(Env *, JsValue *);



