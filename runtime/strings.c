#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "lib/debug.h"
#include "strings.h"
#include "language.h"
#include "gc.h"

#define TEMPLATE_BUFFER_SIZE 4096

// TODO STRING_VALUE_TYPE to ...G_DATA_TY..
typedef struct StringData {
    GcHeader;
    const char* internedString;
    uint64_t length;
    char heapString[];
} StringData;

static bool isInterned(StringData* jsString) {
    return jsString->internedString != NULL;
}

static const char* getCString(StringData* jsString) {
    return isInterned(jsString)
        ? jsString->internedString
        : jsString->heapString;
}

const char* stringGetCString(JsValue* value) {
    return getCString(jsValuePointer(value));
}

JsValue* stringCreateFromInternedString(const char* const interned, uint64_t logicalLength) {

    StringData* jsString;
    JsValue* val; 

    jsValueCreatePointer(val, STRING_TYPE, jsString, STRING_VALUE_TYPE, sizeof(StringData));

    jsString->internedString = interned;
    jsString->length = logicalLength;

    return val;
}

typedef struct StringAllocation {
    JsValue* const string;
    StringData* const data;
} StringAllocation;

static StringAllocation createHeapString(uint64_t stringLength) {
    StringData* data;
    JsValue* value;

    jsValueCreatePointer(value, STRING_TYPE, data, STRING_VALUE_TYPE, sizeof(StringData) + stringLength);

    data->length = stringLength;
    data->internedString = NULL;

    return (StringAllocation) {
        .string = value,
        .data = data,
    };
}

JsValue* stringCreateFromTemplate(const char* format, ...) {
    char buffer[TEMPLATE_BUFFER_SIZE];
    va_list args;
    va_start(args, format);

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
    uint64_t charsWritten = (uint64_t)vsprintf(buffer, format, args);
    va_end(args);
    #pragma clang diagnostic pop

    StringAllocation allocation = createHeapString(charsWritten);
    strcpy(allocation.data->heapString, buffer);

    return allocation.string;
}

JsValue* stringConcat(JsValue* one, JsValue* two) {
    uint64_t newLength = stringLength(one) + stringLength(two);
    StringAllocation allocated = createHeapString(newLength);
    char* destination = allocated.data->heapString;

    strcat(destination, stringGetCString(one));
    strcat(destination, stringGetCString(two));

    return allocated.string;
}

StringData* stringGet(JsValue* value) {
    StringData* jsString = jsValuePointer(value);
    return jsString;
}

bool stringIsEqual(JsValue* left, JsValue* right) {
    if(left == right) {
        return true;
    }

    StringData* leftData = jsValuePointer(left);
    StringData* rightData = jsValuePointer(right);
    return leftData->length == rightData->length
        && strcmp(getCString(leftData), getCString(rightData)) == 0;
}


uint64_t stringLength(JsValue* value) {
    StringData* jsString = jsValuePointer(value);
    return jsString->length;
}

void stringGcTraverse(GcObject* value, GcCallback* cb) {
    cb(jsValuePointer((void*)value));
}

const char* _stringDebugValue(StringData* d) {
    return getCString(d);
}
