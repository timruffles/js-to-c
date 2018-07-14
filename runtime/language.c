#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "language.h"
#include "objects.h"
#include "strings.h"

char UNDEFINED_TYPE[] = "undefined";
char NULL_TYPE[] = "null";
char BOOLEAN_TYPE[] = "boolean";
char OBJECT_TYPE[] = "object";
char STRING_TYPE[] = "string";
char NUMBER_TYPE[] = "number";
char FUNCTION_TYPE[] = "function";

// special - internal types that don't map to a JS primitive type
char NAN_TYPE[] = "NaN";

// Because... why not? Not user mutable so easier to debug
const char TRUE_VALUE = 'Y';
const char FALSE_VALUE = 'N';

// string representations
char TRUE_STRING[] = "true";
char FALSE_STRING[] = "false";
char OBJECT_STRING[] = "[Object object]";
char FUNCTION_STRING[] = "[Function ...]";
char NUMBER_STRING[] = "[todo]";

union JsValueValue {
   double number;
   JsBooleanValue boolean;
   void* pointer;
} value;

// TODO
typedef struct JsValue {
    const char * const type;
    union JsValueValue value;
} JsValue;

JsValue *const TRUE = &(JsValue) {
        .type = BOOLEAN_TYPE,
        .value = {
                .boolean = TRUE_VALUE
        }
};

JsValue *const FALSE = &(JsValue) {
        .type = BOOLEAN_TYPE,
        .value = {
                .boolean = FALSE_VALUE
        }
};

// use pointer equality to check for these values
JsValue *const JS_UNDEFINED = &(JsValue) {
        .type = UNDEFINED_TYPE
};

JsValue *const JS_NULL = &(JsValue) {
        .type = NULL_TYPE
};

JsValue *const JS_NAN = &(JsValue) {
        .type = NAN_TYPE
};

JsValue *getUndefined() {
    return JS_UNDEFINED;
}

JsValue *getNull() {
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
    JsValue* val = calloc(sizeof(JsValue), 1);
    *val = (JsValue) {
        .type = NUMBER_TYPE,
        .value = {
            .number = number, 
        }
    };
    return val;
}

JsValue *jsValueCreatePointer(JsValueType type, void* pointer) {
    JsValue* val = calloc(sizeof(JsValue), 1);
    *val = (JsValue) {
        .type = type,
        .value = {
            .pointer = pointer, 
        }
    };
    return val;
}

char* jsValueToString(JsValue* value) {
    if(value->type == UNDEFINED_TYPE) {
        return UNDEFINED_TYPE;
    } else if(value->type == NULL_TYPE) {
            return NULL_TYPE;
    } else if(value->type == BOOLEAN_TYPE) {
            return (value->value.boolean) == TRUE_VALUE
                ? TRUE_STRING
                : FALSE_STRING;
    } else if(value->type == OBJECT_TYPE) {
            return OBJECT_STRING;
    } else if(value->type == STRING_TYPE) {
            return stringGetCString(value);
    } else if(value->type == NUMBER_TYPE) {
            // TODO interesting - where do we store the stringified value - pass in a char* I suppose
            // and write to that
            return NUMBER_STRING;
    } else {
            return FUNCTION_STRING;
    }
}

void* jsValuePointer(JsValue* val) {
    assert(jsValueType(val) == OBJECT_TYPE || jsValueType(val) == STRING_TYPE);
    // note: this will either be object or string
    return val->value.pointer;
}

double jsValueNumber(JsValue* val) {
    assert(jsValueType(val) == NUMBER_TYPE);
    return val->value.number;
}

JsBooleanValue jsValueBoolean(JsValue* val) {
    assert(jsValueType(val) == BOOLEAN_TYPE);
    return val->value.boolean;
}

bool isUndefined(JsValue *value) {
    return value == JS_UNDEFINED;
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
