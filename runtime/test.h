#pragma once

#include <math.h>

#include "./lib/debug.h"

#define test(T) printf("%s...\n", #T); (T)(); printf(#T" passed\n");

#define assertFloatEqual(A,B) { double a_f_e_result = fabs(A - B); assert(a_f_e_result < 0.000001); }

void testLanguageAndGcInit(void);
