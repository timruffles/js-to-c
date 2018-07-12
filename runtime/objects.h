#pragma once

#include "language.h"

typedef struct JsObject JsObject;

JsValue* objectCreate(JsValue* prototype);
JsValue* objectCreatePlain();
JsValue* objectGet(JsValue* object, JsValue* property);
JsValue* objectSet(JsValue* object, JsValue* property, JsValue* value);
void objectDestroy(JsValue* object);
