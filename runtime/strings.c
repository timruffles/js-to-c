#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "lib/debug.h"
#include "strings.h"
#include "language.h"
#include "gc.h"

#define TEMPLATE_BUFFER_SIZE 4096

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

char* stringGetCString(JsValue* value) {
    return getCString(jsValuePointer(value));
}

JsValue* stringCreateFromInternedString(const char* const interned, uint64_t logicalLength) {
    StringData* jsString = gcAllocate(sizeof(StringData), STRING_VALUE_TYPE);

    jsString->internedString = interned;
    jsString->length = logicalLength;

    return jsValueCreatePointer(STRING_TYPE, jsString);
}

typedef struct AllocatedString {
    JsValue* const string;
    StringData* const data;
} AllocatedString;

static AllocatedString createHeapString(uint64_t stringLength) {
    StringData* data = gcAllocate(sizeof(StringData) + stringLength, STRING_VALUE_TYPE);
    data->length = stringLength;
    data->internedString = NULL;
    JsValue* value = jsValueCreatePointer(STRING_TYPE, data);
    return (AllocatedString) {
        .string = value,
        .data = data,
    };
}

JsValue* stringCreateFromTemplate(const char* format, ...) {
    char buffer[TEMPLATE_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    uint64_t charsWritten = (uint64_t)vsprintf(buffer, format, args);
    va_end(args);

    AllocatedString allocation = createHeapString(charsWritten);
    strcpy(allocation.data->heapString, buffer);

    return allocation.string;
}

JsValue* stringConcat(JsValue* one, JsValue* two) {
    uint64_t newLength = stringLength(one) + stringLength(two);
    AllocatedString allocated = createHeapString(newLength);
    char* destination = allocated.data->heapString;

    strcat(destination, stringGetCString(one));
    strcat(destination, stringGetCString(two));

    return allocated.string;
}

StringData* stringGet(JsValue* value) {
    StringData* jsString = jsValuePointer(value);
    return jsString;
}

bool stringComparison(JsValue* left, JsValue* right) {
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
