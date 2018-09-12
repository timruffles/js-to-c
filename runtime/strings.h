#pragma once
#include <stdbool.h>

#include "language.h"

#define stringFromLiteral(S) stringCreateFromInternedString(S, sizeof(S) - 1)

typedef struct StringData StringData;

JsValue* stringCreateFromInternedString(const char* const, uint64_t size);
StringData* stringGet(JsValue*);
uint64_t stringLength(JsValue*);
const char* stringGetCString(JsValue*);
bool stringIsEqual(JsValue* left, JsValue* right);
JsValue* stringCreateFromTemplate(const char* fmt, ...);

JsValue* stringConcat(JsValue*, JsValue*);
