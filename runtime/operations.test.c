#include <assert.h>
#include <stdio.h>

#include "operations.h"
#include "language.h"
#include "test.h"

static void itHandlesNonNumbersInToNumber() {
    assert(jsValueToNumber(getNaN()).isNaN);
    assert(jsValueToNumber(getUndefined()).isNaN);

    assert(jsValueToNumber(getNull()).isZero);
  
    assert(jsValueToNumber(getTrue()).isOne);
    assert(jsValueToNumber(getFalse()).isZero);
}


int main() {
    testLanguageAndGcInit();

    test(itHandlesNonNumbersInToNumber);
}

