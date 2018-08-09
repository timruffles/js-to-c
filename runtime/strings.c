#include <string.h>
#include <stdlib.h>

#include "strings.h"
#include "language.h"
#include "gc.h"

// our string primitive - can be boxed by toObject
typedef struct StringData {
    char* cString;
    const uint64_t length;
} StringData;

JsValue* stringCreateFromCString(char* string) {
    StringData* jsString = gcAllocate(sizeof(StringData));
    uint64_t l = strlen(string);

    *jsString = (StringData) {
        .cString = string,
        .length = l,
    };
    JsValue *val = jsValueCreatePointer(STRING_TYPE, jsString);
    return val;
}

StringData* stringGet(JsValue* value) {
    StringData* jsString = jsValuePointer(value);
    return jsString;
}

int stringComparison(JsValue* left, JsValue* right) {
    StringData* leftString = jsValuePointer(left);
    StringData* rightString = jsValuePointer(right);
    return strcmp(leftString->cString, rightString->cString);
}

char* stringGetCString(JsValue* value) {
    StringData* jsString = jsValuePointer(value);
    return jsString->cString;
}

uint64_t stringLength(JsValue* value) {
    StringData* jsString = jsValuePointer(value);
    return jsString->length;
}
