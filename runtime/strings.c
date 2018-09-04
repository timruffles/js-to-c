#include <string.h>
#include <stdlib.h>

#include "strings.h"
#include "language.h"
#include "gc.h"

// our string primitive - can be boxed by toObject
typedef struct StringData {
    GcHeader;
    char* cString;
    uint64_t length;
    // if string is interned, we don't manage the memory
    bool interned;
} StringData;

JsValue* stringCreateFromCString(char* string) {
    StringData* jsString = gcAllocate(sizeof(StringData), STRING_VALUE_TYPE);
    uint64_t l = strlen(string);

    jsString->cString = string;
    jsString->length = l;

    JsValue *val = jsValueCreatePointer(STRING_TYPE, jsString);
    return val;
}

JsValue* stringCreateFromTemplate(char* string, ...) {
    // TODO sprintf etc, allocate enough on heap for string
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

JsValue* stringFromTemplate(char* template, ...) {

}
