#pragma once

#include <stdint.h>

#include "language.h"
#include "gc.h"

typedef struct JsArray JsArray;

JsValue* arrayCreate(uint64_t count);
void arrayInitialiseIndex(JsValue* array, JsValue* index, JsValue* val);

JsValue* arrayPutInternal(JsValue* ar, JsValue* key, JsValue* value);
