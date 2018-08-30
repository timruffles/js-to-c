#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "operators.h"
#include "gc.h"
#include "test.h"
#include "language.h"
#include "strings.h"
#include "lib/debug.h"

// float equality - within acceptable error
#define assertDoublesEqual(X,Y) assert(fabs(jsValueNumber(X) - jsValueNumber(Y)) < 0.00000001);
#define jsNum(X) (jsValueCreateNumber(X))

static JsValue* five;
static JsValue* ten;

static void itIdentifiesLessThanOperands() {
    assert(LTOperator(five, ten) == getTrue());
    assert(LTOperator(ten, five) == getFalse());
    assert(LTOperator(ten, ten) == getFalse());
}

static void itIdentifiesLessThanOrEqualOperands() {
    assert(LTEOperator(five, ten) == getTrue());
    assert(LTEOperator(ten, five) == getFalse());
    assert(LTEOperator(ten, ten) == getTrue());
}

static void itIdentifiesGreaterThanOperands() {
    assert(GTOperator(five, ten) == getFalse());
    assert(GTOperator(ten, five) == getTrue());
    assert(GTOperator(ten, ten) == getFalse());
}

static void itIdentifiesGreaterThanOrEqualOperands() {
    assert(GTEOperator(five, ten) == getFalse());
    assert(GTEOperator(ten, five) == getTrue());
    assert(GTEOperator(ten, ten) == getTrue());
}

static void itHasMultiplyOperator() {
    assertDoublesEqual(multiplyOperator(five, ten), jsNum(50));
    assertDoublesEqual(multiplyOperator(ten, five), jsNum(50));

    assertDoublesEqual(multiplyOperator(jsNum(0.3333333333333333), jsNum(3)), jsValueCreateNumber(1));
    assertDoublesEqual(multiplyOperator(jsNum(-3), jsNum( 3)), jsNum(-9));
    assertDoublesEqual(multiplyOperator(jsNum(-3), jsNum(-3)), jsNum( 9));
}

static void itImplementsStrictEquality() {
    assert(strictEqualOperator(getTrue(), getTrue()) == getTrue());
    assert(strictEqualOperator(getTrue(), getFalse()) == getFalse());

    assert(strictEqualOperator(jsNum(5), jsValueCreateNumber(5)) == getTrue());
    assert(strictEqualOperator(jsNum(5), jsValueCreateNumber(10)) == getFalse());
    assert(strictEqualOperator(jsNum(5), jsValueCreateNumber(5.1)) == getFalse());
    assert(strictEqualOperator(five, five) == getTrue());

    assert(strictEqualOperator(stringCreateFromCString("hello"), stringCreateFromCString("hello")) == getTrue());
    assert(strictEqualOperator(stringCreateFromCString("bye"), stringCreateFromCString("hello")) == getFalse());

    assert(strictEqualOperator(jsNum(5), stringCreateFromCString("5")) == getFalse());
    assert(strictEqualOperator(getUndefined(), getFalse()) == getFalse());
    assert(strictEqualOperator(getUndefined(), getNull()) == getFalse());
}

int main() {
    testLanguageAndGcInit();

    five = jsNum(5);
    ten = jsNum(10);

    test(itIdentifiesLessThanOperands);
    test(itIdentifiesLessThanOrEqualOperands);
    test(itIdentifiesGreaterThanOperands);
    test(itIdentifiesGreaterThanOrEqualOperands);
    test(itImplementsStrictEquality);

    test(itHasMultiplyOperator);
}

