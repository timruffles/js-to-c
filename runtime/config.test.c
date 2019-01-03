#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "test.h"
#include "config.h"

static void itHasDefaults() {
    Config config = {
    };
    configInit(&config);
    assert(config.heapSize == HEAP_SIZE_DEFAULT);
}

static void itUsesSetValuesNotDefaults() {
    Config config = {
        .heapSize = 50
    };
    configInit(&config);
    assert(config.heapSize == 50);
}

int main() {
    test(itHasDefaults);
    test(itUsesSetValuesNotDefaults);
}
