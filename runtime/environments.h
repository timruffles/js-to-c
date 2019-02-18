#pragma once

#include "language.h"

typedef JsValue Env;

/**
 * Creates a new environment with the specified parent
 */
Env *envCreateRoot(void);

Env *envFromGlobal(JsValue*);
Env *envCreate(Env *const parent);
Env *envCreateForCall(Env* parent, JsValue* argumentNames[], uint64_t argCount, JsValue* argumentValues[], uint64_t suppliedArgCount);

void envDestroy(Env *);

void envDeclare(Env *, JsValue *name);

void envSet(Env *, JsValue *, JsValue *value);

JsValue *envGet(Env *, JsValue *);



