#pragma once

// forward declare - we're going to have
// a struct called A declare at some point
typedef struct A A;

#include "b.h"

typedef struct A {
    B b;
} A;

