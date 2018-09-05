#pragma once
#include <stdbool.h>

#include "language.h"

typedef struct StringData StringData;

JsValue* stringCreateFromCString(char*);
JsValue* stringCreateFromInternedString(const char* const, uint64_t size);
StringData* stringGet(JsValue*);
uint64_t stringLength(JsValue*);
char* stringGetCString(JsValue*);
bool stringComparison(JsValue* left, JsValue* right);
JsValue* stringCreateFromTemplate(const char* fmt, ...);

JsValue* stringConcat(JsValue*, JsValue*);
