#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "language.h"
#include "test.h"
#include "gc.h"

void itCanCreateAStringFromACString() {
    char* theContent = "Hello";
    JsValue* str = stringCreateFromCString(theContent);

    assert(str != NULL);
}

void itGetsJsStringFromValue() {
    char* theContent = "Hello";
    JsValue* str = stringCreateFromCString(theContent);
    assert(strcmp(stringGetCString(str), theContent) == 0);
}

void itGetsStringLength() {
    char* theContent = "Hello";
    JsValue* str = stringCreateFromCString(theContent);
    assert(stringLength(str) == 5);
}

int main() {
    testLanguageAndGcInit();

    test(itCanCreateAStringFromACString);
    test(itGetsJsStringFromValue);
    test(itGetsStringLength);
}

