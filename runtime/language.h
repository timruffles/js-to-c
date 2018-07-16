#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TO_JS_BOOLEAN(X) ((X) ? getTrue() : getFalse())

// Currently JsIdentifiers are simply c strings - we can know
// all the identifiers we need in environments at compile time
//
// Since we want to keep JsIdentifier as a pointer value, this is
// declared as a char for now
typedef char JsIdentifier;
typedef char JsBooleanValue;

union JsValueValue;

typedef const char* const JsValueType;

extern char UNDEFINED_TYPE[];
extern char NULL_TYPE[];
extern char NUMBER_TYPE[];
extern char BOOLEAN_TYPE[];
extern char OBJECT_TYPE[];
extern char STRING_TYPE[];
extern char FUNCTION_TYPE[];

typedef struct JsValue JsValue;

bool isUndefined(JsValue*);
bool isNaN(JsValue*);

bool isTruthy(JsValue*);

JsValue* getUndefined(void);
JsValue* getNull(void);
JsValue* getNaN(void);

JsValue* getTrue(void);
JsValue* getFalse(void);

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

void jsValueToCString(JsValue*, char* buf, uint64_t bufSize);

JsValueType jsValueType(JsValue* value);

// https://www.ecma-international.org/ecma-262/5.1/#sec-8.7.1
JsValue* getValueOperation(JsValue*);
