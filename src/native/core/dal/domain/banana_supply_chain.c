#include "banana_supply_chain.h"

#include "../internal/banana_postgres.h"

static const char batch_aggregate_type[] = "supply_chain.batch";
static const char shipment_aggregate_type[] = "supply_chain.shipment";

int banana_supply_chain_db_save_batch(const BananaBatch* batch) {
    if (batch == 0 || batch->batch_id.value[0] == '\0') {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_upsert_snapshot(
        batch_aggregate_type,
        batch->batch_id.value,
        batch,
        sizeof(*batch));
}

int banana_supply_chain_db_get_batch(const char* batch_id, BananaBatch* batch) {
    if (batch_id == 0 || batch_id[0] == '\0' || batch == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_get_snapshot(
        batch_aggregate_type,
        batch_id,
        batch,
        sizeof(*batch));
}

int banana_supply_chain_db_clear_batches(void) {
    return banana_postgres_clear_snapshots(batch_aggregate_type);
}

int banana_supply_chain_db_save_shipment(const BananaShipment* shipment) {
    if (shipment == 0 || shipment->shipment_id.value[0] == '\0') {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_upsert_snapshot(
        shipment_aggregate_type,
        shipment->shipment_id.value,
        shipment,
        sizeof(*shipment));
}

int banana_supply_chain_db_get_shipment(const char* shipment_id, BananaShipment* shipment) {
    if (shipment_id == 0 || shipment_id[0] == '\0' || shipment == 0) {
        return BANANA_DB_INVALID_ARGUMENT;
    }

    return banana_postgres_get_snapshot(
        shipment_aggregate_type,
        shipment_id,
        shipment,
        sizeof(*shipment));
}

int banana_supply_chain_db_clear_shipments(void) {
    return banana_postgres_clear_snapshots(shipment_aggregate_type);
}