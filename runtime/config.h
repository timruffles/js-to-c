#pragma once

#define HEAP_SIZE_DEFAULT 1024000

typedef struct {
    // bytes
    uint64_t heapSize;
} Config;

void configInit(Config*);

