#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "test.h"
#include "config.h"


static void itSets() {
    configSet(_TestConfigKey, (ConfigValue) { .uintValue = 7 });
}

static void itGets() {
    configSet(_TestConfigKey, (ConfigValue) { .uintValue = 7 });
    ConfigValue value = configGet(_TestConfigKey);
    assert(value.uintValue == 7);
}

static void itHasDefaults() {
    configInitFromEnv();
    ConfigValue value = configGet(HeapSizeConfig);
    assert(value.uintValue == HEAP_SIZE_DEFAULT);
}

int main() {
    test(itSets);
    test(itGets);
    test(itHasDefaults);
}
