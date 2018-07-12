#pragma once
#include <stdbool.h>

#include "language.h"

// our string primitive - can be boxed by toObject
typedef struct {
    char * const cString;
    const double length;
} JsString;

JsValue* stringCreateFromCString(char*);
