#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d:%s) " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d:%s) " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d:%s:) " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define ensure(T, M) if(!(T)) { log_err(#T); exit(1); }

#define precondition(T, M, ...) if(!(T)) { log_err(M, ##__VA_ARGS__); exit(1); }

#define ensureAlloced(V) if((V) == NULL) { log_err("Could not allocate memory for '%s'", #V); exit(1); }

#define fail(M, ...)  { log_err(M, ##__VA_ARGS__); exit(1); }


#pragma clang diagnostic pop

