#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "language.h"
#include "test.h"
#include "gc.h"

#define INTERNED_STRING(S) stringCreateFromInternedString(S, strlen(S))

static void itCanCreateAStringFromACString() {
    JsValue* str = INTERNED_STRING("Hello");
    assert(str != NULL);
}

static void itGetsJsStringFromValue() {
    char* theContent = "Hello";
    JsValue* str = INTERNED_STRING("Hello");
    assert(strcmp(stringGetCString(str), theContent) == 0);
}

static void itGetsStringLength() {
    char* theContent = "Hello";
    JsValue* str = INTERNED_STRING("Hello");
    assert(strcmp(stringGetCString(str), theContent) == 0);
    assert(stringLength(str) == 5);
}

static void itCreatesStringsFromTemplate() {
    JsValue* str = stringCreateFromTemplate("Hello '%s'", "Javascript");
    assert(stringComparison(str, INTERNED_STRING("Hello 'Javascript'")));
      
}

int main() {
    testLanguageAndGcInit();

    test(itCanCreateAStringFromACString);
    test(itGetsJsStringFromValue);
    test(itGetsStringLength);
    test(itCreatesStringsFromTemplate);
}

