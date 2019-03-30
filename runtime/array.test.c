#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "objects.h"
#include "array.h"
#include "language.h"
#include "strings.h"
#include "test.h"
#include "gc.h"

static void itHasLength() {
    JsValue* ar0 = objectCreateArray(0).value;
    JsValue* ar1 = objectCreateArray(1).value;
    JsValue* ar100 = objectCreateArray(100).value;

    assertJsNumberEqual(JS_GET(ar0, "length"), jsValueCreateNumber(0));
    assertJsNumberEqual(JS_GET(ar1, "length"), jsValueCreateNumber(1));
    assertJsNumberEqual(JS_GET(ar100, "length"), jsValueCreateNumber(100));
}

static void itUpdatesLengthOnAssignment() {
    JsValue* ar0 = objectCreateArray(0).value;
    JsValue* ar1 = objectCreateArray(1).value;
    JsValue* ar100 = objectCreateArray(100).value;

    JsValue* x = stringFromLiteral("x");
    JS_SET(ar0, "0", x);
    JS_SET(ar100, "107", x);
    JS_SET(ar100, "107.99", x);
    objectSet(ar1, jsValueCreateNumber(99), x);

    assertJsNumberEqual(JS_GET(ar0, "length"), jsValueCreateNumber(1));
    assertJsNumberEqual(JS_GET(ar1, "length"), jsValueCreateNumber(100));
    assertJsNumberEqual(JS_GET(ar100, "length"), jsValueCreateNumber(108));
    assertJsNumberEqual(JS_GET(ar100, "length"), jsValueCreateNumber(108));
}

int main() {
    testLanguageAndGcInit();

    test(itHasLength);
    test(itUpdatesLengthOnAssignment);
}
