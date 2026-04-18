#ifndef BANANA_DAL_POSTGRES_H
#define BANANA_DAL_POSTGRES_H

#include "../banana_db.h"

#include <stddef.h>

int banana_postgres_query_legacy_projection(int purchases, int multiplier, int* out_banana);

int banana_postgres_upsert_snapshot(
    const char* aggregate_type,
    const char* aggregate_id,
    const void* snapshot,
    size_t snapshot_size
);

int banana_postgres_get_snapshot(
    const char* aggregate_type,
    const char* aggregate_id,
    void* out_snapshot,
    size_t snapshot_size
);

int banana_postgres_clear_snapshots(const char* aggregate_type);

#endif