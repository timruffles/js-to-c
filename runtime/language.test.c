#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "language.h"

#define test(T) (T)(); printf("%s passed\n", #T);

void itCanCreateNumber() {
    JsValue* val = jsValueCreateNumber(42.42);
    assert(val != NULL);
}

void itCanAccessNumberValue() {
    JsValue* val = jsValueCreateNumber(42.42);
    assert(jsValueNumber(val) == 42.42);
}

void itCanAccessPointerValue() {
    char a = 'Y';

    // just checking if the pointers end up pointing to the right place
    JsValue* val = jsValueCreatePointer(OBJECT_TYPE, &a);
    char* ptr = jsValuePointer(val);

    assert(*ptr == 'Y');
    assert(ptr == &a);
}

int main() {
    test(itCanCreateNumber);
    test(itCanAccessNumberValue);
    test(itCanAccessPointerValue);
}

