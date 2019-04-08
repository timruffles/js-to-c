#pragma once

#include "a.h"

typedef struct {
    // We can only have a pointer
    // here as the value is not yet defined.
    // We can't explicitly have structs
    // that store each other as the size 
    // calculations are circular
    A* a;
    double dbl;
} B;
