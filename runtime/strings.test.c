#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "strings.h"
#include "language.h"

#define test(T) (T)(); printf("%s passed\n", #T);

void itCanCreateAStringFromACString() {
    char* theContent = "Hello";
    JsValue* str = stringCreateFromCString(theContent);

    assert(str != NULL);
}

int main() {
    test(itCanCreateAStringFromACString);
}

