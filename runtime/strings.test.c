#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "language.h"
#include "test.h"
#include "gc.h"

#define internedString(S) stringCreateFromInternedString(S, strlen(S))
#define assertCStringValue(S,CS) assert(strcmp(stringGetCString(S), CS) == 0);

static void itCanCreateAStringFromACString() {
    JsValue* str = internedString("Hello");
    assert(str != NULL);
}

static void itGetsJsStringFromValue() {
    char* theContent = "Hello";
    JsValue* str = internedString("Hello");
    assert(strcmp(stringGetCString(str), theContent) == 0);
}

static void itGetsStringLength() {
    char* theContent = "Hello";
    JsValue* str = internedString("Hello");
    assert(strcmp(stringGetCString(str), theContent) == 0);
    assert(stringLength(str) == 5);

    JsValue* emptyString = internedString("");
    assert(stringLength(emptyString) == 0);
}

static void itCreatesStringsFromTemplate() {
    JsValue* str = stringCreateFromTemplate("Hello '%s'", "Javascript");
    assert(stringComparison(str, internedString("Hello 'Javascript'")));
      
}

static void itJoinsStrings() {
    JsValue* joined = stringConcat(internedString("One"), internedString("Two"));
    assertCStringValue(joined, "OneTwo");
    assert(stringComparison(joined, internedString("OneTwo")));
    assert(stringLength(joined) == 6);
}

static void itJoinsInternedAndDynamicStrings() {
    JsValue* joined = stringConcat(internedString("One"), stringCreateFromTemplate("%s", "TwoP"));
    assertCStringValue(joined, "OneTwoP");
}

int main() {
    testLanguageAndGcInit();

    test(itCanCreateAStringFromACString);
    test(itGetsJsStringFromValue);
    test(itGetsStringLength);
    test(itCreatesStringsFromTemplate);
    test(itJoinsStrings);
    test(itJoinsInternedAndDynamicStrings);
}

