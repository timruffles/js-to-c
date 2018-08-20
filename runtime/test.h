#pragma once

#include "./lib/debug.h"

#define test(T) printf("%s...\n", #T); (T)(); printf(#T" passed\n");

void testLanguageAndGcInit(void);
