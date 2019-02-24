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
    // Check if the pointer ends up pointing to the right place.
    //
    // This is a weird test as all of the pointer type constructors conceal their
    // struct's shapes, so manually point to a string rather than correct type for testing.
    // Also the is a lower level bit of the system so don't want to rely on a concrete type.
    char* justForTest = "Y";
    JsValue* val;
    JsObject* obj;
    jsValueCreatePointer(val, OBJECT_TYPE, obj, OBJECT_VALUE_TYPE, 1);

    void* assingWrongTypeForEaseOfTesting = (void*)obj;
    assingWrongTypeForEaseOfTesting = justForTest;

    assertStringEqual(jsValuePointer(val), "Y");
}

static void itHasDefinedTruthiness() {
    assert(isTruthy(getTrue()));
    assert(isTruthy(stringFromLiteral("hello")));
    assert(isTruthy(jsValueCreateNumber(7)));
    assert(isTruthy(objectCreatePlain()));

    assert(!isTruthy(jsValueCreateNumber(0)));
    assert(!isTruthy(getFalse()));
    assert(!isTruthy(getUndefined()));
    assert(!isTruthy(getNull()));
    assert(!isTruthy(stringFromLiteral("")));
}

static void itIdentifiesPrimitives() {
    assert(jsValueIsPrimitive(getTrue()));
    assert(jsValueIsPrimitive(getUndefined()));
    assert(jsValueIsPrimitive(getNull()));
    assert(jsValueIsPrimitive(stringFromLiteral("hi")));

    assert(!jsValueIsPrimitive(objectCreatePlain()));
}

int main() {
    testLanguageAndGcInit();

    test(itCanCreateNumber);
    test(itCanAccessNumberValue);
    test(itCanAccessPointerValue);

    test(itHasDefinedTruthiness);
    test(itIdentifiesPrimitives);
}

