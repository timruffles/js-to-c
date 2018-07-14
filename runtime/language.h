#pragma once

#include <stdint.h>
#include <stdbool.h>

// Currently JsIdentifiers are simply c strings - we can know
// all the identifiers we need in environments at compile time
//
// Since we want to keep JsIdentifier as a pointer value, this is
// declared as a char for now
typedef char JsIdentifier;
typedef char JsBooleanValue;

union JsValueValue;

typedef char JsValueType[];

extern char UNDEFINED_TYPE[];
extern char NULL_TYPE[];
extern char BOOLEAN_TYPE[];
extern char OBJECT_TYPE[];
extern char STRING_TYPE[];
extern char FUNCTION_TYPE[];

typedef struct JsValue JsValue;

// shared singleton constant values
extern JsValue* const UNDEFINED;

bool isUndefined(JsValue* val);
JsValue* getUndefined();

JsValue* getTrue();
JsValue* getFalse();

/**
 * These are used internally by the actual concrete values
 * - e.g objects.c, strings.c
 *
 * Value is specified as a ptr value as it's either a 
 * numeric or boolean type which both fit within a pointer,
 * or a pointer to a value for all other types.
 */
JsValue *jsValueCreatePointer(JsValueType type, void*);
JsValue *jsValueCreateNumber(double);

double jsValueNumber(JsValue* value);
void* jsValuePointer(JsValue* value);

char* jsValueToString(JsValue*);
