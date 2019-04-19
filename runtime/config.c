#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "lib/debug.h"
#include "config.h"

static void setHeapSizeFromEnv(Config* config) {
    if(config->heapSize != 0) {
        return;
    }

    // set in kibibytes
    char* heapSizeRaw = getenv("JSC_HEAP_SIZE_KB");
    if(heapSizeRaw == NULL) {
        log_info("No heap size env var");
        goto setDefault;
    }

    int heapSize = atoi(heapSizeRaw);
    if(heapSize <= 0) {
        log_info("Invalid heap size setting %d", heapSize);
        goto setDefault;
    }

    config->heapSize = (uint64_t)heapSize * 1024;
    return;

setDefault:;
    config->heapSize = HEAP_SIZE_DEFAULT;
}

void configInit(Config* config) {
    setHeapSizeFromEnv(config);
}

