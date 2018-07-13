#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "objects.h"
#include "language.h"
#include "strings.h"
#include "test.h"

JsValue* idOne;
JsValue* idOneB;
JsValue* idOneC;
JsValue* idTwo;

void itCreatesPlainObjects() {
    JsValue* object = objectCreatePlain();
    assert(object != NULL);
    free(object);
}

void itDestroysObjects() {
    JsValue* object = objectCreatePlain();
    assert(object != NULL);
    objectDestroy(object);
}


void itAssignsProperties() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    objectDestroy(object);
}

void itGetsPropertyValues() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    assert(objectGet(object, idOne) == getTrue());
    assert(objectGet(object, idOne) != getFalse());

    objectDestroy(object);
}

void itLooksUpByStringValueNotIdentity() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    assert(objectGet(object, idOneB) == getTrue());
    assert(objectGet(object, idOneC) == getTrue());

    objectDestroy(object);
}

void itReturnsUndefinedForMissingKeys() {
    JsValue* object = objectCreatePlain();
    objectSet(object, idOne, getTrue());

    assert(objectGet(object, idTwo) == getUndefined());

    objectDestroy(object);
}

void itUpdatesPropertyValues() {
    JsValue* object = objectCreatePlain();

    objectSet(object, idOne, getTrue());
    assert(objectGet(object, idOne) == getTrue());
    
    objectSet(object, idOne, getFalse());
    assert(objectGet(object, idOne) == getFalse());

    objectDestroy(object);
}

void itCreatesObjectsWithPrototype() {
    JsValue* prototype = objectCreatePlain();
    JsValue* object = objectCreate(prototype);

    assert(object != NULL);
}

void itFindsPrototypeProperties() {
    JsValue* prototype = objectCreatePlain();
    objectSet(prototype, idOne, getTrue());
    JsValue* object = objectCreate(prototype);

    assert(objectGet(object, idOne) == getTrue());

    assert(object != NULL);
}

void itPrefersOwnPropertiesToPrototype() {
    JsValue* prototype = objectCreatePlain();
    objectSet(prototype, idOne, getTrue());
    JsValue* object = objectCreate(prototype);
    objectSet(object, idOne, getFalse());

    assert(objectGet(object, idOne) == getFalse());

    assert(object != NULL);
}

int main() {
    idOne = stringCreateFromCString("one");
    idOneB = stringCreateFromCString("one");
    idOneC = stringCreateFromCString("one");
    idTwo = stringCreateFromCString("two");

    test(itCreatesPlainObjects)
    test(itDestroysObjects)
    test(itAssignsProperties)
    test(itGetsPropertyValues)
    test(itUpdatesPropertyValues);
    test(itCreatesObjectsWithPrototype);
    test(itFindsPrototypeProperties);
    test(itPrefersOwnPropertiesToPrototype);
    test(itReturnsUndefinedForMissingKeys);
    test(itLooksUpByStringValueNotIdentity);
}

