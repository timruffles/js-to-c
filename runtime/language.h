#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gc.h"

#define TO_JS_BOOLEAN(X) ((X) ? getTrue() : getFalse())

enum TypeTag {
    // these are the types for root JS values
    UNDEFINED_TYPE,
    NULL_TYPE,
    NUMBER_TYPE,
    BOOLEAN_TYPE,
    OBJECT_TYPE,
    STRING_TYPE,
    FUNCTION_TYPE,
    // internal types that don't map to a JS primitive type
    NAN_TYPE,

    // other allocated types that aren't explicitly
    // user allocated - implementation details
    STRING_VALUE_TYPE,
    OBJECT_VALUE_TYPE,
    PROPERTY_DESCRIPTOR_TYPE,
};

typedef struct GcObjectReflection {
    char name[24];
} GcObjectReflection;

typedef int JsValueType;

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
 */
JsValue *jsValueCreatePointer(JsValueType type, void*);
JsValue *jsValueCreateNumber(double);

double jsValueNumber(JsValue* value);
void* jsValuePointer(JsValue* value);
void jsValuePointerSet(JsValue* val, void* ptr);

void jsValueToCString(JsValue*, char* buf, uint64_t bufSize);

JsValueType jsValueType(JsValue* value);
JsValue* jsValueMovedTo(JsValue* value);

// https://www.ecma-international.org/ecma-262/5.1/#sec-8.7.1
JsValue* getValueOperation(JsValue*);

GcObjectReflection jsValueReflect(JsValue*);
GcObjectReflection gcObjectReflect(GcObject*);
