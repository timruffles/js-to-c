#pragma once
#include <stdbool.h>

#include "language.h"

typedef struct StringData StringData;

JsValue* stringCreateFromCString(char*);
StringData* stringGet(JsValue*);
uint64_t stringLength(JsValue*);
char* stringGetCString(JsValue*);
int stringComparison(JsValue* left, JsValue* right);
