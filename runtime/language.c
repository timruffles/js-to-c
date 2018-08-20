#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "lib/debug.h"
#include "language.h"
#include "objects.h"
#include "strings.h"
#include "gc.h"

// Because... why not? Not user mutable so easier to debug
static const char TRUE_VALUE = 'Y';
static const char FALSE_VALUE = 'N';

// string representations
static char TRUE_STRING[] = "true";
static char FALSE_STRING[] = "false";
static char OBJECT_STRING[] = "[Object object]";
static char FUNCTION_STRING[] = "[Function ...]";

union JsValueValue {
   const double number;
   const char boolean;
   const void* pointer;
   const GcObject* objectPointer;
};

typedef struct JsValue {
    GcHeader;
    union JsValueValue value;
} JsValue;

// use pointer equality to check for these values
static JsValue* TRUE;
static JsValue* FALSE;
static JsValue* JS_UNDEFINED;
static JsValue* JS_NULL;
static JsValue* JS_NAN;

JsValue* getUndefined() {
    return JS_UNDEFINED;
}

JsValue* getNull() {
    return JS_NULL;
}

JsValue *getNaN() {
    return JS_NAN;
}

bool isNaN(JsValue* value) {
    return JS_NAN == value;
}

JsValue *getTrue() {
    return TRUE;
}

JsValue *getFalse() {
    return FALSE;
}

JsValue *jsValueCreateNumber(double number) {
    JsValue* val = gcAllocate2(sizeof(JsValue), NUMBER_TYPE);
    val->value = (union JsValueValue){ .number = number };
    return val;
}

JsValue *jsValueCreatePointer(JsValueType type, void* pointer) {
    JsValue* val = gcAllocate2(sizeof(JsValue), type);
    val->value.pointer = pointer;
    return val;
}

#define OUTPUT_CONST(X) snprintf(outputBuffer, bufferSize, X);
void jsValueToCString(JsValue* value, char* outputBuffer, uint64_t bufferSize) {
    if(value->type == UNDEFINED_TYPE) {
        OUTPUT_CONST("undefined");
    } else if(value->type == NULL_TYPE) {
        OUTPUT_CONST("null");
    } else if(value->type == BOOLEAN_TYPE) {
        OUTPUT_CONST((value->value.boolean) == TRUE_VALUE
            ? TRUE_STRING
            : FALSE_STRING);
    } else if(value->type == OBJECT_TYPE) {
        OUTPUT_CONST(OBJECT_STRING);
    } else if(value->type == STRING_TYPE) {
        OUTPUT_CONST(stringGetCString(value));
    } else if(value->type == NUMBER_TYPE) {
        snprintf(outputBuffer, bufferSize, "%f", jsValueNumber(value));
    } else {
        OUTPUT_CONST(FUNCTION_STRING);
    }
}

void* jsValuePointer(JsValue* val) {
    assert(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE || jsValueType(val) == FUNCTION_TYPE);
    // note: this will either be object or string
    return (void*)val->value.pointer;
}

void jsValuePointerSet(JsValue* val, void* ptr) {
    assert(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE || jsValueType(val) == FUNCTION_TYPE);
    // note: this will either be object or string
    val->value.pointer = ptr;
}

double jsValueNumber(JsValue* val) {
    assert(jsValueType(val) == NUMBER_TYPE);
    return val->value.number;
}

bool isUndefined(JsValue *value) {
    return value == JS_UNDEFINED;
}

bool isTruthy(JsValue *value) {
    if(value->type == UNDEFINED_TYPE) {
        return false;
    } else if(value->type == NULL_TYPE) {
        return false;
    } else if(value->type == BOOLEAN_TYPE) {
        log_info("V %p %i %c", value, value->type, value->value.boolean);
        return value == getTrue();
    } else if(value->type == OBJECT_TYPE) {
        return true;
    } else if(value->type == STRING_TYPE) {
        // TODO yah yah whitespace is falsy
        return stringLength(value) > 0;
    } else if(value->type == NUMBER_TYPE) {
        return jsValueNumber(value) != 0;
    } else {
        // functions
        return true;
    }
}

JsValue* getValueOperation(JsValue* value) {
    if(value->type == NUMBER_TYPE) {
        return value;
    } else {
        // TODO just for now
        return jsValueCreateNumber(0);
    }
}

JsValueType jsValueType(JsValue* value) {
    return value->type;
}

void languageInit() {
    assert(!"See notes - makes much more sense to special case these in GC");
    TRUE = gcAllocate2(sizeof(JsValue), BOOLEAN_TYPE);
    TRUE->value = (union JsValueValue){ .boolean = TRUE_VALUE };

    FALSE = gcAllocate2(sizeof(JsValue), BOOLEAN_TYPE);
    FALSE->value = (union JsValueValue){ .boolean = FALSE_VALUE };

    // use pointer equality to check for these values
    JS_UNDEFINED = gcAllocate2(sizeof(JsValue), UNDEFINED_TYPE);
    JS_NAN = gcAllocate2(sizeof(JsValue), NAN_TYPE);
    JS_NULL = gcAllocate2(sizeof(JsValue), NULL_TYPE);
}


#define REFLECT(T, S) case T: return (GcObjectReflection) { .name = S };
GcObjectReflection jsValueReflect(JsValue* object) {
    switch(object->type) {
        REFLECT(UNDEFINED_TYPE, "undefined");
        REFLECT(NULL_TYPE, "null");
        REFLECT(NUMBER_TYPE, "number");
        REFLECT(BOOLEAN_TYPE, "boolean");
        REFLECT(OBJECT_TYPE, "object");
        REFLECT(STRING_TYPE, "string");
        REFLECT(FUNCTION_TYPE, "function");
        default:
            assert(false);
    }
}

GcObjectReflection gcObjectReflect(GcObject* object) {
    switch(object->type) {
        REFLECT(UNDEFINED_TYPE, "undefined");
        REFLECT(NULL_TYPE, "null");
        REFLECT(NUMBER_TYPE, "number");
        REFLECT(BOOLEAN_TYPE, "boolean");
        REFLECT(OBJECT_TYPE, "object");
        REFLECT(STRING_TYPE, "string");
        REFLECT(FUNCTION_TYPE, "function");

        REFLECT(STRING_VALUE_TYPE, "stringValue");
        REFLECT(OBJECT_VALUE_TYPE, "objectValue");
        REFLECT(PROPERTY_DESCRIPTOR_TYPE, "propertyDescriptor");

        default:
            assert(false);
    }
}

