#pragma once

#include <stddef.h>

#define BANANA_MAX_BATCHES 128
#define BANANA_MAX_HARVEST_BATCHES 128

typedef struct BananaBatchRecord {
    char id[32];
    int item_count_estimate;
    char ripeness_hint[16];
} BananaBatchRecord;

typedef struct HarvestBatchRecord {
    char id[32];
    int bunch_count;
} HarvestBatchRecord;
