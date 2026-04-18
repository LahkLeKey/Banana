#include "banana_cultivation.h"

#include "../internal/banana_postgres.h"

static const char plant_aggregate_type[] = "cultivation.plant";

int banana_cultivation_db_save_plant(const BananaPlant* plant) {
    if (plant == 0 || plant->plant_id.value[0] == '\0') {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_upsert_snapshot(
        plant_aggregate_type,
        plant->plant_id.value,
        plant,
        sizeof(*plant));
}

int banana_cultivation_db_get_plant(const char* plant_id, BananaPlant* plant) {
    if (plant_id == 0 || plant_id[0] == '\0' || plant == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_get_snapshot(
        plant_aggregate_type,
        plant_id,
        plant,
        sizeof(*plant));
}

int banana_cultivation_db_clear_plants(void) {
    return banana_postgres_clear_snapshots(plant_aggregate_type);
}