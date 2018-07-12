#include <stdbool.h>

#include "language.h"


char UNDEFINED_TYPE[] = "undefined";
char NULL_TYPE[] = "null";
char BOOLEAN_TYPE[] = "boolean";
char OBJECT_TYPE[] = "object";

// use pointer equality to check for these values
JsValue *const UNDEFINED = &(JsValue) {
        .type = UNDEFINED_TYPE
};

// because... why not?
const int TRUE_VALUE = 'Y';
const int FALSE_VALUE = 'N';

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

JsValue *getUndefined() {
    return UNDEFINED;
}

JsValue *getTrue() {
    return TRUE;
}

JsValue *getFalse() {
    return FALSE;
}

bool isUndefined(JsValue *value) {
    return value == UNDEFINED;
}

