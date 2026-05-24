#include "tick_budget_policy.h"

#include <stdlib.h>

int runtime_tick_budget_policy_terrain_chunks_per_tick(void)
{
    const char *raw = getenv("BANANA_TERRAIN_REBUILD_BUDGET");
    long value = 2;

    if (raw && raw[0])
        value = strtol(raw, NULL, 10);

    if (value < 1)
        return 1;
    if (value > 128)
        return 128;

    return (int)value;
}
