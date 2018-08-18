#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "test.h"
#include "config.h"


static void itSets() {
    setConfig(_TestConfigKey, (ConfigValue) { .uintValue = 7 });
}

static void itGets() {
    setConfig(_TestConfigKey, (ConfigValue) { .uintValue = 7 });
    ConfigValue value = getConfig(_TestConfigKey);
    assert(value.uintValue == 7);
}

static void itHasDefaults() {
    initConfigFromEnv();
    ConfigValue value = getConfig(HeapSizeConfig);
    assert(value.uintValue == HEAP_SIZE_DEFAULT);
}

int main() {
    test(itSets);
    test(itGets);
    test(itHasDefaults);
}
