#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gcObject.h"

#define TO_JS_BOOLEAN(X) ((X) ? getTrue() : getFalse())
#define DEBUG_VALUE_N(M, X, O) char* op##O = calloc(1, 1024); jsValueToCString(X, op##O, 1000); printf(M, op##O);


enum TypeTag {
    // reserve 0 for cases where a JsValue is uninitialized, or pointing to cleared memory
    UNITIALIZED_TYPE,

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
    FUNCTION_RECORD_TYPE,

    FREE_SPACE_TYPE,
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
bool jsValueIsPrimitive(JsValue* value);

void jsValueToCString(JsValue*, char* buf, uint64_t bufSize);

JsValueType jsValueType(JsValue* value);

// https://www.ecma-international.org/ecma-262/5.1/#sec-8.7.1
JsValue* getValueOperation(JsValue*);

GcObjectReflection jsValueReflect(JsValue*);
GcObjectReflection gcObjectReflect(GcObject*);
GcObjectReflection gcObjectReflectType(int type);

// debugging macro to ensure a typeX() function is called on something sensible
#define languageAssertType(V,T) precondition(V != NULL, "null pointer"); if(jsValueType((void*)V) != T) { fail("expected %s got %s", jsValueReflect((void*)V).name, gcObjectReflectType(T).name); }


