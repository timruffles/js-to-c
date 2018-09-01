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

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d:%s: errno: %s) " M "\n", __FILE__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d:%s: errno: %s) " M "\n", __FILE__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d:%s:) " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#define todo(M) log_err(M); goto fail;

#define ensure(T) if(!(T)) { log_err(#T); exit(1); }

#define ensureAlloced(V) if((V) == NULL) { log_err("Could not allocate memory for '%s'", #V); exit(1); }

#define fail(M, ...)  { log_err(M, ##__VA_ARGS__); exit(1); }




#pragma clang diagnostic pop

