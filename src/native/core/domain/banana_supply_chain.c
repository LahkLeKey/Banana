#include "banana_supply_chain.h"

#include "../dal/banana_db.h"

#include <string.h>

static BananaBatch batch_registry[BANANA_MAX_BATCH_REGISTRY];
static int batch_registry_count = 0;

static int find_batch_index(const char* batch_id) {
    int index = 0;

    if (!banana_identifier_is_valid(batch_id)) {
        return -1;
    }

    for (index = 0; index < batch_registry_count; index++) {
        if (strcmp(batch_registry[index].batch_id.value, batch_id) == 0) {
            return index;
        }
    }

    return -1;
}

static int coordinates_are_valid(BananaGeoCoordinates location) {
    return location.latitude >= -90.0
        && location.latitude <= 90.0
        && location.longitude >= -180.0
        && location.longitude <= 180.0;
}

static int identifier_index(
    const BananaIdentifier* identifiers,
    int count,
    const char* value
) {
    int index = 0;

    if (identifiers == 0 || value == 0) {
        return -1;
    }

    for (index = 0; index < count; index++) {
        if (strcmp(identifiers[index].value, value) == 0) {
            return index;
        }
    }

    return -1;
}

static int export_transition_is_valid(BananaExportStatus current, BananaExportStatus next_status) {
    if (current == BANANA_EXPORT_PACKED && next_status == BANANA_EXPORT_SHIPPED) {
        return 1;
    }

    if (current == BANANA_EXPORT_SHIPPED && next_status == BANANA_EXPORT_CUSTOMS) {
        return 1;
    }

    if (current == BANANA_EXPORT_CUSTOMS && next_status == BANANA_EXPORT_DISTRIBUTED) {
        return 1;
    }

    return 0;
}

static BananaStatus cache_batch(const BananaBatch* batch) {
    int index = -1;

    if (batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = find_batch_index(batch->batch_id.value);
    if (index >= 0) {
        batch_registry[index] = *batch;
        return BANANA_OK;
    }

    if (batch_registry_count >= BANANA_MAX_BATCH_REGISTRY) {
        return BANANA_ERROR_OVERFLOW;
    }

    batch_registry[batch_registry_count] = *batch;
    batch_registry_count++;
    return BANANA_OK;
}

static void persist_batch_state(const BananaBatch* batch) {
    if (batch == 0 || batch->batch_id.value[0] == '\0') {
        return;
    }

    (void)cache_batch(batch);
    (void)banana_db_save_batch(batch);
}

BananaStatus banana_batch_register(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    BananaBatch* batch
) {
    BananaBatch candidate;
    BananaStatus status = BANANA_OK;

    if (batch == 0
        || storage_temp_c < 0.0
        || storage_temp_c > 60.0
        || ethylene_exposure < 0.0
        || estimated_shelf_life_days < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(&candidate, 0, sizeof(candidate));

    status = banana_identifier_copy(&candidate.batch_id, batch_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&candidate.origin_farm, origin_farm);
    if (status != BANANA_OK) {
        return status;
    }

    candidate.bunch_count = 0;
    candidate.export_status = BANANA_EXPORT_PACKED;
    candidate.storage_temp_c = storage_temp_c;
    candidate.ethylene_exposure = ethylene_exposure;
    candidate.estimated_shelf_life_days = estimated_shelf_life_days;

    status = cache_batch(&candidate);
    if (status != BANANA_OK) {
        return status;
    }

    *batch = candidate;
    (void)banana_db_save_batch(&candidate);
    return BANANA_OK;
}

BananaStatus banana_batch_get(
    const char* batch_id,
    BananaBatch* batch
) {
    int db_status = BANANA_DB_NOT_CONFIGURED;
    int index = -1;

    if (batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (batch_id != 0 && batch_id[0] != '\0') {
        db_status = banana_db_get_batch(batch_id, batch);
        if (db_status == BANANA_DB_OK) {
            (void)cache_batch(batch);
            return BANANA_OK;
        }
    }

    index = find_batch_index(batch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *batch = batch_registry[index];
    return BANANA_OK;
}

BananaStatus banana_batch_add_bunch(
    BananaBatch* batch,
    const char* bunch_id
) {
    BananaStatus status = BANANA_OK;

    if (batch == 0 || batch->export_status != BANANA_EXPORT_PACKED) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (batch->bunch_count >= BANANA_MAX_BUNCHES_PER_BATCH
        || identifier_index(batch->bunch_ids, batch->bunch_count, bunch_id) >= 0) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&batch->bunch_ids[batch->bunch_count], bunch_id);
    if (status != BANANA_OK) {
        return status;
    }

    batch->bunch_count++;
    persist_batch_state(batch);
    return BANANA_OK;
}

BananaStatus banana_batch_advance_export_status(
    BananaBatch* batch,
    BananaExportStatus next_status
) {
    if (batch == 0 || !export_transition_is_valid(batch->export_status, next_status)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    batch->export_status = next_status;
    persist_batch_state(batch);
    return BANANA_OK;
}

BananaStatus banana_distribution_node_register(
    const char* node_id,
    DistributionNodeType type,
    BananaGeoCoordinates location,
    double capacity_kg,
    DistributionNode* node
) {
    BananaStatus status = BANANA_OK;

    if (node == 0 || capacity_kg <= 0.0 || !coordinates_are_valid(location)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(node, 0, sizeof(*node));

    status = banana_identifier_copy(&node->node_id, node_id);
    if (status != BANANA_OK) {
        return status;
    }

    node->type = type;
    node->location = location;
    node->capacity_kg = capacity_kg;
    node->inventory_batch_count = 0;
    return BANANA_OK;
}

BananaStatus banana_distribution_node_receive_batch(
    DistributionNode* node,
    const char* batch_id
) {
    BananaStatus status = BANANA_OK;

    if (node == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (node->inventory_batch_count >= BANANA_MAX_BATCHES_PER_NODE
        || identifier_index(node->inventory_batches, node->inventory_batch_count, batch_id) >= 0) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&node->inventory_batches[node->inventory_batch_count], batch_id);
    if (status != BANANA_OK) {
        return status;
    }

    node->inventory_batch_count++;
    return BANANA_OK;
}

BananaStatus banana_distribution_node_dispatch_batch(
    DistributionNode* node,
    const char* batch_id
) {
    int index = -1;
    int current = 0;

    if (node == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = identifier_index(node->inventory_batches, node->inventory_batch_count, batch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    for (current = index; current < node->inventory_batch_count - 1; current++) {
        node->inventory_batches[current] = node->inventory_batches[current + 1];
    }

    memset(&node->inventory_batches[node->inventory_batch_count - 1], 0, sizeof(BananaIdentifier));
    node->inventory_batch_count--;
    return BANANA_OK;
}

BananaStatus banana_shipment_create(
    const char* shipment_id,
    const char* origin_node_id,
    const char* destination_node_id,
    BananaShipment* shipment
) {
    BananaStatus status = BANANA_OK;

    if (shipment == 0 || origin_node_id == 0 || destination_node_id == 0 || strcmp(origin_node_id, destination_node_id) == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(shipment, 0, sizeof(*shipment));

    status = banana_identifier_copy(&shipment->shipment_id, shipment_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&shipment->origin_node_id, origin_node_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&shipment->destination_node_id, destination_node_id);
    if (status != BANANA_OK) {
        return status;
    }

    shipment->status = BANANA_SHIPMENT_PLANNED;
    shipment->batch_count = 0;
    shipment->departure_day_ordinal = -1;
    shipment->arrival_day_ordinal = -1;
    return BANANA_OK;
}

BananaStatus banana_shipment_add_batch(
    BananaShipment* shipment,
    const BananaBatch* batch
) {
    BananaStatus status = BANANA_OK;

    if (shipment == 0 || batch == 0 || shipment->status != BANANA_SHIPMENT_PLANNED) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (shipment->batch_count >= BANANA_MAX_BATCHES_PER_SHIPMENT
        || identifier_index(shipment->batch_ids, shipment->batch_count, batch->batch_id.value) >= 0) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&shipment->batch_ids[shipment->batch_count], batch->batch_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    shipment->batch_count++;
    return BANANA_OK;
}

BananaStatus banana_shipment_dispatch(
    BananaShipment* shipment,
    BananaBatch* batch,
    int departure_day_ordinal,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (shipment == 0
        || batch == 0
        || departure_day_ordinal < 0
        || shipment->status != BANANA_SHIPMENT_PLANNED
        || identifier_index(shipment->batch_ids, shipment->batch_count, batch->batch_id.value) < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_batch_advance_export_status(batch, BANANA_EXPORT_SHIPPED);
    if (status != BANANA_OK) {
        return status;
    }

    shipment->status = BANANA_SHIPMENT_IN_TRANSIT;
    shipment->departure_day_ordinal = departure_day_ordinal;
    return banana_domain_event_record(
        event,
        BANANA_EVENT_SHIPPED,
        batch->batch_id.value,
        shipment->shipment_id.value,
        BANANA_STAGE_GREEN,
        batch->bunch_count,
        departure_day_ordinal,
        0.0);
}

BananaStatus banana_shipment_arrive(
    BananaShipment* shipment,
    BananaBatch* batch,
    int arrival_day_ordinal,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (shipment == 0
        || batch == 0
        || arrival_day_ordinal < shipment->departure_day_ordinal
        || shipment->status != BANANA_SHIPMENT_IN_TRANSIT
        || identifier_index(shipment->batch_ids, shipment->batch_count, batch->batch_id.value) < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_batch_advance_export_status(batch, BANANA_EXPORT_CUSTOMS);
    if (status != BANANA_OK) {
        return status;
    }

    shipment->status = BANANA_SHIPMENT_ARRIVED;
    shipment->arrival_day_ordinal = arrival_day_ordinal;
    return banana_domain_event_record(
        event,
        BANANA_EVENT_ARRIVED,
        batch->batch_id.value,
        shipment->shipment_id.value,
        BANANA_STAGE_GREEN,
        batch->bunch_count,
        arrival_day_ordinal,
        0.0);
}

BananaStatus banana_batch_predict_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    BananaRipenessPrediction* prediction
) {
    BananaBatch batch;
    BananaRipenessInput input;
    BananaStatus status = BANANA_OK;

    if (prediction == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_batch_get(batch_id, &batch);
    if (status != BANANA_OK) {
        return status;
    }

    input.temperature_history_c = temperature_history_c;
    input.temperature_history_count = temperature_history_count;
    input.days_since_harvest = days_since_harvest;
    input.ethylene_exposure = batch.ethylene_exposure;
    input.mechanical_damage = mechanical_damage;
    input.storage_temp_c = batch.storage_temp_c;

    status = banana_predict_ripeness(&input, prediction);
    if (status != BANANA_OK) {
        return status;
    }

    if (batch.estimated_shelf_life_days > 0) {
        int batch_limit_hours = batch.estimated_shelf_life_days * 24;
        if (prediction->shelf_life_hours > batch_limit_hours) {
            prediction->shelf_life_hours = batch_limit_hours;
        }
    }

    return BANANA_OK;
}