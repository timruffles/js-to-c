#pragma once

#include "language.h"

typedef struct JsObject JsObject;

JsValue* objectCreate(JsValue* prototype);
JsValue* objectCreatePlain(void);
JsValue* objectGet(JsValue* object, JsValue* property);
JsValue* objectSet(JsValue* object, JsValue* property, JsValue* value);
void objectDestroy(JsValue* object);

// used internally by environments to model envs as objects
JsValue* objectLookup(JsValue* object, JsValue* property);
