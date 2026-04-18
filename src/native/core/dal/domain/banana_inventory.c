#include "banana_inventory.h"

#include "../internal/banana_postgres.h"

static const char inventory_item_aggregate_type[] = "inventory.item";

int banana_inventory_db_save_item(const BananaInventoryItem* item) {
    if (item == 0 || item->inventory_id.value[0] == '\0') {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_upsert_snapshot(
        inventory_item_aggregate_type,
        item->inventory_id.value,
        item,
        sizeof(*item));
}

int banana_inventory_db_get_item(const char* inventory_id, BananaInventoryItem* item) {
    if (inventory_id == 0 || inventory_id[0] == '\0' || item == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_get_snapshot(
        inventory_item_aggregate_type,
        inventory_id,
        item,
        sizeof(*item));
}

int banana_inventory_db_clear_items(void) {
    return banana_postgres_clear_snapshots(inventory_item_aggregate_type);
}