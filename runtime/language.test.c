#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "language.h"
#include "objects.h"
#include "strings.h"
#include "gc.h"
#include "test.h"

void itCanCreateNumber() {
    JsValue* val = jsValueCreateNumber(42.42);
    assert(val != NULL);
}

void itCanAccessNumberValue() {
    JsValue* val = jsValueCreateNumber(42.42);
    assert(jsValueNumber(val) == 42.42);
}

void itCanAccessPointerValue() {
    // just checking if the pointers end up pointing to the right place
    JsPointerAllocation alloc = jsValueCreatePointer(OBJECT_TYPE, sizeof(char[64]));

    assert(alloc.pointer != NULL);
    assert(alloc.value != NULL);
    assert(alloc.value->type == OBJECT_TYPE);
}

void itHasDefinedTruthiness() {
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
    _gcTestInit();

    test(itCanCreateNumber);
    test(itCanAccessNumberValue);
    test(itCanAccessPointerValue);

    test(itHasDefinedTruthiness);
}

