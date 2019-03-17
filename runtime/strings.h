#pragma once
#include <stdbool.h>

#include "language.h"
#include "gc.h"

// sizeof("x") is 2, as it includes the room for NULL byte
#define stringFromLiteral(S) stringCreateFromInternedString(S, sizeof(S) - 1)

typedef struct StringData StringData;

JsValue* stringCreateFromInternedString(const char* const, uint64_t size);
StringData* stringGet(JsValue*);
uint64_t stringLength(JsValue*);
char* stringGetCString(JsValue*);
bool stringIsEqual(JsValue* left, JsValue* right);
JsValue* stringCreateFromTemplate(const char* fmt, ...);

JsValue* stringConcat(JsValue*, JsValue*);

void stringGcTraverse(GcObject*, GcCallback*);

const char* _stringDebugValue(StringData*);
