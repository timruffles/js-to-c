#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "lib/debug.h"
#include "language.h"
#include "objects.h"
#include "strings.h"
#include "gc.h"


union JsValueValue {
   double number;
   void* pointer;
};

typedef struct JsValue {
    GcHeader;
    union JsValueValue value;
} JsValue;

GcObjectReflection gcObjectReflectType(int type);

static JsValue *const TRUE = &(JsValue) {
        .type = BOOLEAN_TYPE,
};

static JsValue *const FALSE = &(JsValue) {
        .type = BOOLEAN_TYPE,
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

static JsValue *const JS_INFINITY = &(JsValue) {
        .type = NUMBER_TYPE,
        .value = (union JsValueValue){
            .number = (double)INFINITY,
        }
};

// 1 and 0 are returned from spec operations (ToNumber etc), so useful
// to allocate once and reference statically
static JsValue *const JS_ZERO = &(JsValue) {
    .type = NUMBER_TYPE,
    .value = (union JsValueValue){
        .number = 0,
    }
};

static JsValue *const JS_ONE = &(JsValue) {
    .type = NUMBER_TYPE,
    .value = (union JsValueValue){
        .number = 1,
    }
};

JsValue* getInfinity() {
    return JS_INFINITY;
}

JsValue* getOne() {
    return JS_ONE;
}

JsValue* getZero() {
    return JS_ZERO;
}

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

/**
 * Use via jsValueCreatePointer macro that ensure value and reference are atomically allocated.
 */
JsValue *_jsValueCreatePointer(JsValueType type, void* pointer) {
    JsValue* val = gcAllocate(sizeof(JsValue), type);
    val->value.pointer = pointer;
    return val;
}

// ToString operation - https://www.ecma-international.org/ecma-262/5.1/#sec-9.8
JsValue* jsValueToString(JsValue* value) {
    // TODO Really stringifiers for each type should request memory from gc as they know they need it (e.g Array.toString())
    // TODO all the static strings should be allocated once
    if(value->type == UNDEFINED_TYPE) {
        return stringFromLiteral("undefined");
    } else if(value->type == NULL_TYPE) {
        return stringFromLiteral("null");
    } else if(value->type == BOOLEAN_TYPE) {
        return value == getTrue()
            ? stringFromLiteral("true")
            : stringFromLiteral("false");
    } else if(value->type == OBJECT_TYPE) {
        return stringFromLiteral("[Object object]");
    } else if(value->type == STRING_TYPE) {
        return value;
    } else if(value->type == NUMBER_TYPE) {
        if(jsValueNumber(value) == (double)INFINITY) {
            return stringFromLiteral("Infinity");
        } else {
            return stringCreateFromTemplate("%g", jsValueNumber(value));
        }
    } else if(value->type == NAN_TYPE) {
        return stringFromLiteral("NaN");
    } else {
        return stringFromLiteral("[Function ...]");
    }
}

// dereference the pointed-to value of non-immediate JS Value
// TODO rename - jsValueAsPointer?
void* jsValuePointer(JsValue* val) {
    precondition(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE || jsValueType(val) == FUNCTION_TYPE, "Expected pointer value, got %s at %p", gcObjectReflectType(jsValueType(val)).name, val);
    return val->value.pointer;
}

void jsValuePointerSet(JsValue* val, void* ptr) {
    precondition(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE || jsValueType(val) == FUNCTION_TYPE, "Expected pointer value, got %s", gcObjectReflectType(jsValueType(val)).name);
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
// NOTE: typeof should be implemented explcitly in operators
GcObjectReflection jsValueReflect(JsValue* object) {
    switch(object->type) {
        REFLECT(UNDEFINED_TYPE, "undefined");
        REFLECT(NULL_TYPE, "null");
        REFLECT(NUMBER_TYPE, "number");
        REFLECT(NAN_TYPE, "number");
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
        REFLECT(NAN_TYPE, "nan");

        REFLECT(STRING_VALUE_TYPE, "stringValue");
        REFLECT(OBJECT_VALUE_TYPE, "objectValue");
        REFLECT(PROPERTY_DESCRIPTOR_TYPE, "propertyDescriptor");
        REFLECT(FUNCTION_RECORD_TYPE, "functionRecord");

        REFLECT(FREE_SPACE_TYPE, "freeSpace");

        default:
            log_err("Unknown type %i", type);
            assert(false);
    }

}

GcObjectReflection gcObjectReflect(GcObject* object) {
    return gcObjectReflectType(object->type);
}


