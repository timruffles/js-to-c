#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "lib/debug.h"
#include "language.h"
#include "objects.h"
#include "strings.h"
#include "gc.h"

char UNDEFINED_TYPE[] = "undefined";
char NULL_TYPE[] = "null";
char BOOLEAN_TYPE[] = "boolean";
char OBJECT_TYPE[] = "object";
char STRING_TYPE[] = "string";
char NUMBER_TYPE[] = "number";
char FUNCTION_TYPE[] = "function";

// special - internal types that don't map to a JS primitive type
static char NAN_TYPE[] = "NaN";

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
   JsBooleanValue boolean;
   void* pointer;
};

typedef struct JsValue {
    const char * const type;
    JsValue* movedTo;
    union JsValueValue value;
} JsValue;

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
    JsValue* val = gcAllocate(sizeof(JsValue));
    *val = (JsValue) {
        .type = NUMBER_TYPE,
        .value = {
            .number = number,
        }
    };
    return val;
}

JsPointerAllocation jsValueCreatePointer(JsValueType type, size_t size) {
    JsValue* val = gcAllocate(sizeof(JsValue));
    void* pointer = gcAllocate(size);
    *val = (JsValue) {
        .type = type,
        .value = {
            .pointer = pointer,
        }
    };
    return (JsPointerAllocation) {
        .pointer = pointer,
        .value = val,
    };
}

#define OUTPUT_CONST(X) snprintf(outputBuffer, bufferSize, X);
void jsValueToCString(JsValue* value, char* outputBuffer, uint64_t bufferSize) {
    if(value->type == UNDEFINED_TYPE) {
        OUTPUT_CONST(UNDEFINED_TYPE);
    } else if(value->type == NULL_TYPE) {
        OUTPUT_CONST(NULL_TYPE);
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
    return val->value.pointer;
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

JsValueType jsValueType(JsValue* value) {
    return value->type;
}

