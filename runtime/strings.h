#pragma once
#include <stdbool.h>

#include "language.h"

typedef struct JsString JsString;

JsValue* stringCreateFromCString(char*);
JsString* stringGet(JsValue*);
uint64_t stringLength(JsValue*);
char* stringGetCString(JsValue*);
