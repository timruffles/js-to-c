#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "language.h"
#include "objects.h"
#include "strings.h"
#include "gc.h"
#include "test.h"

static void itCanCreateNumber() {
    JsValue* val = jsValueCreateNumber(42.42);
    assert(val != NULL);
}

static void itCanAccessNumberValue() {
    JsValue* val = jsValueCreateNumber(42.42);
    assert(jsValueNumber(val) == 42.42);
}

static void itCanAccessPointerValue() {
    char a = 'Y';

    // just checking if the pointers end up pointing to the right place
    JsValue* val = jsValueCreatePointer(OBJECT_TYPE, &a);
    char* ptr = jsValuePointer(val);

    assert(*ptr == 'Y');
    assert(ptr == &a);
}

static void itHasDefinedTruthiness() {
    assert(isTruthy(getTrue()));
    assert(isTruthy(stringCreateFromCString("hello")));
    assert(isTruthy(jsValueCreateNumber(7)));
    assert(isTruthy(objectCreatePlain()));

    assert(!isTruthy(jsValueCreateNumber(0)));
    assert(!isTruthy(getFalse()));
    assert(!isTruthy(getUndefined()));
    assert(!isTruthy(getNull()));
    assert(!isTruthy(stringCreateFromCString("")));
}

int main() {
    testLanguageAndGcInit();

    test(itCanCreateNumber);
    test(itCanAccessNumberValue);
    test(itCanAccessPointerValue);

    test(itHasDefinedTruthiness);
}

