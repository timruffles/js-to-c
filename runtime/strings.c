#include <string.h>
#include <stdlib.h>

#include "strings.h"
#include "language.h"

// our string primitive - can be boxed by toObject
typedef struct JsString {
    char* cString;
    const uint64_t length;
} JsString;

JsValue* stringCreateFromCString(char* string) {
    JsString* jsString = calloc(1, sizeof(JsString));
    uint64_t l = strlen(string);

    *jsString = (JsString) {
        .cString = string,
        .length = l,
    };
    JsValue *val = jsValueCreatePointer(STRING_TYPE, jsString);
    return val;
}

JsString* stringGet(JsValue* value) {
    JsString* jsString = jsValuePointer(value);
    return jsString;
}

int stringComparison(JsValue* left, JsValue* right) {
    JsString* leftString = jsValuePointer(left);
    JsString* rightString = jsValuePointer(right);
    return strcmp(leftString->cString, rightString->cString);
}

char* stringGetCString(JsValue* value) {
    JsString* jsString = jsValuePointer(value);
    return jsString->cString;
}

uint64_t stringLength(JsValue* value) {
    JsString* jsString = jsValuePointer(value);
    return jsString->length;
}
