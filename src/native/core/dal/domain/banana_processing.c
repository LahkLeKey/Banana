#include "banana_processing.h"

#include "../internal/banana_postgres.h"

static const char bunch_aggregate_type[] = "processing.bunch";

int banana_processing_db_save_bunch(const BananaBunchRecord* bunch) {
    if (bunch == 0 || bunch->aggregate.bunch.bunch_id.value[0] == '\0') {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_upsert_snapshot(
        bunch_aggregate_type,
        bunch->aggregate.bunch.bunch_id.value,
        bunch,
        sizeof(*bunch));
}

int banana_processing_db_get_bunch(const char* bunch_id, BananaBunchRecord* bunch) {
    if (bunch_id == 0 || bunch_id[0] == '\0' || bunch == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_get_snapshot(
        bunch_aggregate_type,
        bunch_id,
        bunch,
        sizeof(*bunch));
}

int banana_processing_db_clear_bunches(void) {
    return banana_postgres_clear_snapshots(bunch_aggregate_type);
}