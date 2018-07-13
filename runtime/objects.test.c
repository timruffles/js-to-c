#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "objects.h"
#include "language.h"
#include "strings.h"

#define test(T) printf("%s...", #T); (T)(); printf(" passed\n");

JsValue* idOne;


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

void itUpdatesPropertyValues() {
    JsValue* object = objectCreatePlain();

    objectSet(object, idOne, getTrue());
    assert(objectGet(object, idOne) == getTrue());
    
    objectSet(object, idOne, getFalse());
    assert(objectGet(object, idOne) == getFalse());

    objectDestroy(object);
}

int main() {
    idOne = stringCreateFromCString("one");

    test(itCreatesPlainObjects)
    test(itDestroysObjects)
    test(itAssignsProperties)
    test(itGetsPropertyValues)
    test(itUpdatesPropertyValues);
}

