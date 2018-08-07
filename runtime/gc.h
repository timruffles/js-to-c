#include <stdint.h>

typedef struct {
    uint64_t used;
    uint64_t remaining;
} GcStats;

void gcInit(void);
void* gcAllocate(size_t);
GcStats gcStats(void);

void _gcTestInit(void);
