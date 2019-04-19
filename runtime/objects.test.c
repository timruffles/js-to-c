#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "objects.h"
#include "language.h"
#include "strings.h"
#include "test.h"
#include "gc.h"

static JsValue* idOne;
static JsValue* idOneB;
static JsValue* idOneC;
static JsValue* idTwo;

static void itCreatesPlainObjects() {
    JsValue* object = objectCreatePlain();
    assert(object != NULL);
}

static void itDestroysObjects() {
    JsValue* object = objectCreatePlain();
    assert(object != NULL);
    objectDestroy(object);
}


static void itAssignsProperties() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    objectDestroy(object);
}

static void itGetsPropertyValues() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    assert(objectGet(object, idOne) == getTrue());
    assert(objectGet(object, idOne) != getFalse());

    objectDestroy(object);
}

static void itGetsPropertyValuesWithMultiple() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());
    objectSet(object, idTwo, getTrue());

    assert(objectGet(object, idTwo) == getTrue());
}

static void itLooksUpByStringValueNotIdentity() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    assert(objectGet(object, idOneB) == getTrue());
    assert(objectGet(object, idOneC) == getTrue());

    objectDestroy(object);
}

static void itReturnsUndefinedForMissingKeys() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    assert(objectGet(object, idTwo) == getUndefined());

    objectDestroy(object);
}

static void itUpdatesPropertyValues() {
    JsValue* object = objectCreatePlain();

    objectSet(object, idOne, getTrue());
    assert(objectGet(object, idOne) == getTrue());
    
    objectSet(object, idOne, getFalse());
    assert(objectGet(object, idOne) == getFalse());

    objectDestroy(object);
}

static void itCreatesObjectsWithPrototype() {
    JsValue* prototype = objectCreatePlain();
    JsValue* object = objectCreate(prototype);

    assert(object != NULL);
}

static void itFindsPrototypeProperties() {
    JsValue* prototype = objectCreatePlain();
    objectSet(prototype, idOne, getTrue());
    JsValue* object = objectCreate(prototype);

    assert(objectGet(object, idOne) == getTrue());

    assert(object != NULL);
}

static void itPrefersOwnPropertiesToPrototype() {
    JsValue* prototype = objectCreatePlain();
    objectSet(prototype, idOne, getTrue());
    JsValue* object = objectCreate(prototype);
    objectSet(object, idOne, getFalse());

    assert(objectGet(object, idOne) == getFalse());

    assert(object != NULL);
}

static void itSupportsForOwnIteration() {
    JsValue* obj = objectCreatePlain();
    objectSet(obj, stringFromLiteral("one"), jsValueCreateNumber(1.0));
    objectSet(obj, stringFromLiteral("two"), jsValueCreateNumber(2.5));

    ForOwnIterator iterator = objectForOwnPropertiesIterator(obj);
    double result = 0;
    while(iterator.property) {
        result += jsValueNumber(objectGet(obj, iterator.property));
        iterator = objectForOwnPropertiesNext(iterator);
    }

    assertFloatEqual(result, 3.5);
}

int main() {
    testLanguageAndGcInit();

    idOne = stringFromLiteral("one");
    idOneB = stringFromLiteral("one");
    idOneC = stringFromLiteral("one");
    idTwo = stringFromLiteral("two");

    test(itCreatesPlainObjects)
    test(itDestroysObjects)
    test(itAssignsProperties)
    test(itGetsPropertyValues)
    test(itGetsPropertyValuesWithMultiple);
    test(itUpdatesPropertyValues);
    test(itCreatesObjectsWithPrototype);
    test(itFindsPrototypeProperties);
    test(itPrefersOwnPropertiesToPrototype);
    test(itReturnsUndefinedForMissingKeys);
    test(itLooksUpByStringValueNotIdentity);
    test(itSupportsForOwnIteration);
}

