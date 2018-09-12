#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "language.h"
#include "test.h"
#include "gc.h"

#define assertCStringValue(S,CS) assert(strcmp(stringGetCString(S), CS) == 0);

static void itCanCreateAStringFromACString() {
    JsValue* str = stringFromLiteral("Hello");
    assert(str != NULL);
}

static void itGetsJsStringFromValue() {
    char* theContent = "Hello";
    JsValue* str = stringFromLiteral("Hello");
    assert(strcmp(stringGetCString(str), theContent) == 0);
}

static void itGetsStringLength() {
    char* theContent = "Hello";
    JsValue* str = stringFromLiteral("Hello");
    assert(strcmp(stringGetCString(str), theContent) == 0);
    assert(stringLength(str) == 5);

    JsValue* emptyString = stringFromLiteral("");
    assert(stringLength(emptyString) == 0);
}

static void itComparesStrings() {
    assert(stringIsEqual(stringFromLiteral("hello"), stringFromLiteral("hello")));
    assert(!stringIsEqual(stringFromLiteral("Hello"), stringFromLiteral("hello")));
    assert(stringIsEqual(stringFromLiteral("hello"), stringCreateFromTemplate("%s", "hello")));
    assert(stringIsEqual(stringFromLiteral("A"), stringFromLiteral("A")));
}

static void itCreatesStringsFromTemplate() {
    JsValue* str = stringCreateFromTemplate("Hello '%s'", "Javascript");
    assert(stringIsEqual(str, stringFromLiteral("Hello 'Javascript'")));
}

static void itJoinsStrings() {
    JsValue* joined = stringConcat(stringFromLiteral("One"), stringFromLiteral("Two"));
    assertCStringValue(joined, "OneTwo");
    assert(stringIsEqual(joined, stringFromLiteral("OneTwo")));
    assert(stringLength(joined) == 6);
}

static void itJoinsInternedAndDynamicStrings() {
    JsValue* joined = stringConcat(stringFromLiteral("One"), stringCreateFromTemplate("%s", "TwoP"));
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
    test(itComparesStrings);
}

