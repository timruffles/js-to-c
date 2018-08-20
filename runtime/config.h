#pragma once

#define HEAP_SIZE_DEFAULT 1024

typedef enum ConfigKey {
    HeapSizeConfig,
    _TestConfigKey,
} ConfigKey;

typedef union ConfigValue {
    const uint64_t uintValue;
} ConfigValue;

void configSet(ConfigKey key, ConfigValue value);
ConfigValue configGet(ConfigKey key);
void configInitFromEnv(void);

