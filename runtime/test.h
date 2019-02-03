#pragma once

#include <math.h>

#include "./lib/debug.h"

#define test(T) printf("%s...\n", #T); (T)(); printf(#T" passed\n");

#define assertFloatEqual(A,B) { double a_f_e_result = fabs(A - B); assert(a_f_e_result < 0.000001); }
#define refuteFloatEqual(A,B) { double a_f_e_result = fabs(A - B); assert(a_f_e_result > 0.000001); }
#define assertEqual(A,B,F) { if(A != B) { log_err("Expected "#F" == " #F, A,B); exit(1); } }

#define assertStringEqual(A,B) { int r = strcmp(A,B) == 0; if(!r) { log_err("Expected \"%s\" == \"%s\"", A, B); exit(1); } }

#define assertEqual_int(A,B) assertEqual(A,B,%p)
#define assertEqual_uint64_t(A,B) assertEqual(A,B,%p)
#define assertPointersEqual(A,B) assertEqual(A,B,%p)

void testLanguageAndGcInit(void);


