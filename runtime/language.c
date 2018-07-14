#include <stdbool.h>
#include <stdlib.h>

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
                .boolean = TRUE_VALUE
        }
};

// use pointer equality to check for these values
JsValue *const UNDEFINED = &(JsValue) {
        .type = UNDEFINED_TYPE
};

JsValue *getUndefined() {
    return UNDEFINED;
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
    } else if(value->type == FUNCTION_TYPE) {
            return FUNCTION_STRING;
    }
}

void* jsValuePointer(JsValue* val) {
    // note: this will either be object or string
    return val->value.pointer;
}

double jsValueNumber(JsValue* val) {
    return val->value.number;
}

JsBooleanValue jsValueBoolean(JsValue* val) {
    return val->value.boolean;
}

bool isUndefined(JsValue *value) {
    return value == UNDEFINED;
}

