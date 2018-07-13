#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "strings.h"
#include "language.h"
#include "test.h"

void itCanCreateAStringFromACString() {
    char* theContent = "Hello";
    JsValue* str = stringCreateFromCString(theContent);

    assert(str != NULL);
}

void itGetsJsStringFromValue() {
    char* theContent = "Hello";
    JsValue* str = stringCreateFromCString(theContent);
    JsString* jsStr = stringGet(str);
    assert(strcmp(jsStr->cString, theContent) == 0);
}

int main() {
    test(itCanCreateAStringFromACString);
    test(itGetsJsStringFromValue);
}

