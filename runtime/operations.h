#pragma once
#include <stdbool.h>

#include "language.h"

typedef struct {
    double number;
    bool isNaN:1;
    bool isZero:1;
    bool isOne:1;
} ToNumberResult;

ToNumberResult jsValueToNumber(JsValue*);
