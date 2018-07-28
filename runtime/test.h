#pragma once

#define test(T) printf("%s...\n", #T); (T)(); printf(#T" passed\n");
