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
   double number;
   const char boolean;
   void* pointer;
   GcObject* objectPointer;
};

typedef struct JsValue {
    GcHeader;
    union JsValueValue value;
} JsValue;

GcObjectReflection gcObjectReflectType(int type);

static JsValue *const TRUE = &(JsValue) {
        .type = BOOLEAN_TYPE,
        .value = {
                .boolean = TRUE_VALUE
        }
};

static JsValue *const FALSE = &(JsValue) {
        .type = BOOLEAN_TYPE,
        .value = {
                .boolean = FALSE_VALUE
        }
};

// use pointer equality to check for these values
static JsValue *const JS_UNDEFINED = &(JsValue) {
        .type = UNDEFINED_TYPE
};

static JsValue *const JS_NULL = &(JsValue) {
        .type = NULL_TYPE
};

static JsValue *const JS_NAN = &(JsValue) {
        .type = NAN_TYPE
};

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
    JsValue* val = gcAllocate(sizeof(JsValue), NUMBER_TYPE);
    val->value.number = number;
    return val;
}

JsValue *jsValueCreatePointer(JsValueType type, void* pointer) {
    JsValue* val = gcAllocate(sizeof(JsValue), type);
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
    precondition(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE || jsValueType(val) == FUNCTION_TYPE, "Expected pointer value, got %s", gcObjectReflectType(jsValueType(val)).name);
    // note: this will either be object or string
    return val->value.pointer;
}

void jsValuePointerSet(JsValue* val, void* ptr) {
    precondition(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE || jsValueType(val) == FUNCTION_TYPE, "Expected pointer value, got %s", gcObjectReflectType(jsValueType(val)).name);
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

bool jsValueIsPrimitive(JsValue* value) {
    switch(value->type) {
        case OBJECT_TYPE:
        case FUNCTION_TYPE:
            return false;

        case UNDEFINED_TYPE:
        case NULL_TYPE:
        case NUMBER_TYPE:
        case BOOLEAN_TYPE:
        case STRING_TYPE:
            return true;

        default:
            fail("Non JSValue %s", gcObjectReflectType(value->type).name);
    }
}

JsValueType jsValueType(JsValue* value) {
    return value->type;
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
            fail("Non JSValue %s", gcObjectReflectType(object->type).name);
    }
}

GcObjectReflection gcObjectReflectType(int type) {
    switch(type) {
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
        REFLECT(FUNCTION_RECORD_TYPE, "functionRecord");

        REFLECT(FREE_SPACE_TYPE, "free space");

        default:
            log_err("Unknown type %i", type);
            assert(false);
    }

}
GcObjectReflection gcObjectReflect(GcObject* object) {
    return gcObjectReflectType(object->type);
}

