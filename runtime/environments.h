#pragma once

#include "language.h"

typedef struct Env Env;

/**
 * Creates a new environment with the specified parent
 */
Env *envCreateRoot();

Env *envCreate(Env *const parent);

void envDestroy(Env *);

void envDeclare(Env *, JsIdentifier *name);

void envSet(Env *, JsIdentifier *, JsValue *value);

JsValue *envGet(Env *, JsIdentifier *);


/**
 * Test helpers - wrapping string interning
 */
#define _identifierCreate(X) char X[] = #X;
