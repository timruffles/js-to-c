#pragma once

#define HEAP_SIZE_DEFAULT 1024000

typedef enum ConfigKey {
    HeapSizeConfig,
    _TestConfigKey,
} ConfigKey;

typedef union ConfigValue {
    const uint64_t uintValue;
} ConfigValue;

void setConfig(ConfigKey key, ConfigValue value);
ConfigValue getConfig(ConfigKey key);
void initConfigFromEnv();

