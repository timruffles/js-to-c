#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "lib/debug.h"
#include "config.h"

typedef struct ConfigRecord {
    ConfigKey key;
    ConfigValue value;
    struct ConfigRecord* next;
} ConfigRecord;

static ConfigRecord* configRecords;
static ConfigRecord* tailRecord;

void configSet(ConfigKey key, ConfigValue value) {
    for(ConfigRecord* head = configRecords;
        head != NULL;
        head = head->next) {
        if(head->key == key) {
            head->value = value;
            return;
        }
    }

    ConfigRecord* newRecord = calloc(1, sizeof(ConfigRecord));
    *newRecord = (ConfigRecord) {
        .key = key,
        .value = value,
    };
    if(configRecords == NULL) {
        configRecords = newRecord;
    } else {
        tailRecord->next = newRecord;
    }
    tailRecord = newRecord;
}

ConfigValue configGet(ConfigKey key) {
    for(ConfigRecord* head = configRecords;
        head != NULL;
        head = head->next) {
        if(head->key == key) {
            return head->value;
        }
    }
    assert(!"failed to find config value");
}

static void setHeapSize() {
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

    ConfigValue value = { .uintValue = (uint64_t)heapSize * 1024 };
    configSet(HeapSizeConfig, value);
    return;

setDefault:;
    ConfigValue defaultValue = { .uintValue = HEAP_SIZE_DEFAULT };
    configSet(HeapSizeConfig, defaultValue);
}

void configInitFromEnv() {
    // yes, this leaks, no biggy just in tests
    configRecords = NULL;
    tailRecord = NULL;

    setHeapSize();
}

