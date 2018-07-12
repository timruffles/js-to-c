#pragma once

#include <stdint.h>
#include <stdbool.h>


// Currently JsIdentifiers are simply c strings - we can know
// all the identifiers we need in environments at compile time
//
// Since we want to keep JsIdentifier as a pointer value, this is
// declared as a char for now
typedef char JsIdentifier;
typedef int JsBooleanValue;

// object creation is handled by objects.c
extern char OBJECT_TYPE[];


// TODO
typedef struct JsValue {
    char *type;
    union {
       double number;
       JsBooleanValue boolean;
       JsObject* object;
    } value;
} JsValue;

// shared singleton constant values
extern JsValue* const UNDEFINED;

bool isUndefined(JsValue* val);
JsValue* getUndefined();

JsValue* getTrue();
JsValue* getFalse();

/**
 * Test helpers - hiding implementation of string interning
 */
#define _createInternedStringForTest(X) char X[] = #X;
