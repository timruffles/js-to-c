#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "operators.h"
#include "test.h"
#include "language.h"

JsValue* five;
JsValue* ten;

void itIdentifiesLessThanOperands() {
    assert(LTOperator(five, ten) == getTrue());
    assert(LTOperator(ten, five) == getFalse());
    assert(LTOperator(ten, ten) == getFalse());
}

void itIdentifiesLessThanOrEqualOperands() {
    assert(LTEOperator(five, ten) == getTrue());
    assert(LTEOperator(ten, five) == getFalse());
    assert(LTEOperator(ten, ten) == getTrue());
}

void itIdentifiesGreaterThanOperands() {
    assert(GTOperator(five, ten) == getFalse());
    assert(GTOperator(ten, five) == getTrue());
    assert(GTOperator(ten, ten) == getFalse());
}

void itIdentifiesGreaterThanOrEqualOperands() {
    assert(GTEOperator(five, ten) == getFalse());
    assert(GTEOperator(ten, five) == getTrue());
    assert(GTEOperator(ten, ten) == getTrue());
}

int main() {
    five = jsValueCreateNumber(5);
    ten = jsValueCreateNumber(10);

    test(itIdentifiesLessThanOperands);
    test(itIdentifiesLessThanOrEqualOperands);
    test(itIdentifiesGreaterThanOperands);
    test(itIdentifiesGreaterThanOrEqualOperands);
}

