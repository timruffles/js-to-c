#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "language.h"

#define test(T) (T)(); printf("%s passed\n", #T);

void itCanCreateNumber() {
    JsValue* val = jsValueCreateNumber(BOOLEAN_TYPE, 42.42);
    assert(val != NULL);
}

void itCanAccessNumberValue() {
    JsValue* val = jsValueCreate(BOOLEAN_TYPE, 'Y');
    uintptr_t p = jsValueGet(val);
    char castVal = (char) jsValueGet(val);
    printf("thing '%c' or %p\n", castVal, p);

    char* c = &p;
    for(uint16_t i = 0; i < 8; i++) {
        printf("thing byte %d: %c\n", i, *(c+i));
    }
    assert(castVal == 'Y');
}

int main() {
    test(itCanCreateValue);
    test(itCanAccessValue);
}

