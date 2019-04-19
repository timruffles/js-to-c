#pragma once

#include <math.h>

#include "./lib/debug.h"

#define test(T) printf("%s...\n", #T); (T)(); printf(#T" passed\n");

#define assertJsNumberEqual(A,B) { assertFloatEqual(jsValueNumber(A), jsValueNumber(B)) }
#define assertFloatEqual(A,B) { double a_f_e_result = fabs(A - B); precondition(a_f_e_result < 0.000001, "expected %g %g to equal", A, B); }
#define refuteFloatEqual(A,B) { double a_f_e_result = fabs(A - B); precondition(a_f_e_result > 0.000001, "expected floats not to equal"); }
#define assertEqual(A,B,F) { precondition(A == B, "Expected "#F" == " #F, A, B); }

#define assertStringEqual(A,B) { int r = strcmp(A,B) == 0; if(!r) { log_err("Expected \"%s\" == \"%s\"", A, B); assert(0); } }
#define assertJsStringValue(J,S) assertStringEqual(stringGetCString(J), S)

#define assertEqual_int(A,B) assertEqual(A,B,%p)
#define assertEqual_uint64_t(A,B) assertEqual(A,B,%p)
#define assertPointersEqual(A,B) assertEqual(A,B,%p)

void testLanguageAndGcInit(void);


