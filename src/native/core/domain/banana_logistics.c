#include "banana_logistics.h"

#include <string.h>

static int coordinates_are_valid(BananaGeoCoordinates location) {
    return location.latitude >= -90.0
        && location.latitude <= 90.0
        && location.longitude >= -180.0
        && location.longitude <= 180.0;
}

static int identifier_index(const BananaIdentifier* identifiers, int count, const char* value) {
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

static double estimate_batch_weight_kg(const BananaBatch* batch) {
    int effective_bunch_count = 1;

    if (batch != 0 && batch->bunch_count > 0) {
        effective_bunch_count = batch->bunch_count;
    }

    return (double)effective_bunch_count * 18.0;
}

BananaStatus banana_temperature_setting_validate(const BananaTemperatureSetting* setting) {
    if (setting == 0
        || setting->set_point_c < 0.0
        || setting->set_point_c > 60.0
        || setting->tolerance_c < 0.0
        || setting->tolerance_c > 10.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    return BANANA_OK;
}

BananaStatus banana_current_location_set(
    const char* node_id,
    BananaGeoCoordinates coordinates,
    DistributionNodeType node_type,
    BananaCurrentLocation* location
) {
    BananaStatus status = BANANA_OK;

    if (location == 0 || !coordinates_are_valid(coordinates)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(location, 0, sizeof(*location));
    status = banana_identifier_copy(&location->node_id, node_id);
    if (status != BANANA_OK) {
        return status;
    }

    location->coordinates = coordinates;
    location->node_type = node_type;
    return BANANA_OK;
}

BananaStatus banana_container_register(
    const char* container_id,
    BananaCurrentLocation location,
    BananaTemperatureSetting temperature,
    double capacity_kg,
    BananaContainer* container
) {
    BananaStatus status = BANANA_OK;

    if (container == 0 || capacity_kg <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_temperature_setting_validate(&temperature);
    if (status != BANANA_OK) {
        return status;
    }

    memset(container, 0, sizeof(*container));
    status = banana_identifier_copy(&container->container_id, container_id);
    if (status != BANANA_OK) {
        return status;
    }

    container->current_location = location;
    container->temperature = temperature;
    container->capacity_kg = capacity_kg;
    return BANANA_OK;
}

BananaStatus banana_container_load_batch(
    BananaContainer* container,
    const BananaBatch* batch
) {
    BananaStatus status = BANANA_OK;
    double estimated_weight_kg = 0.0;

    if (container == 0 || batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (container->batch_count >= BANANA_MAX_BATCHES_PER_CONTAINER
        || identifier_index(container->batch_ids, container->batch_count, batch->batch_id.value) >= 0) {
        return BANANA_ERROR_OVERFLOW;
    }

    estimated_weight_kg = estimate_batch_weight_kg(batch);
    if (container->current_weight_kg + estimated_weight_kg > container->capacity_kg) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&container->batch_ids[container->batch_count], batch->batch_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    container->batch_count++;
    container->current_weight_kg += estimated_weight_kg;
    return BANANA_OK;
}

BananaStatus banana_container_unload_batch(
    BananaContainer* container,
    const char* batch_id
) {
    int index = -1;
    int current = 0;

    if (container == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = identifier_index(container->batch_ids, container->batch_count, batch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    for (current = index; current < container->batch_count - 1; current++) {
        container->batch_ids[current] = container->batch_ids[current + 1];
    }

    memset(&container->batch_ids[container->batch_count - 1], 0, sizeof(BananaIdentifier));
    container->batch_count--;
    if (container->current_weight_kg >= 18.0) {
        container->current_weight_kg -= 18.0;
    } else {
        container->current_weight_kg = 0.0;
    }

    return BANANA_OK;
}

BananaStatus banana_ripening_room_register(
    const char* room_id,
    BananaCurrentLocation location,
    double target_temp_c,
    double target_ethylene,
    BananaRipenessStage target_stage,
    BananaRipeningRoom* room
) {
    BananaStatus status = BANANA_OK;

    if (room == 0
        || target_temp_c < 0.0
        || target_temp_c > 60.0
        || target_ethylene < 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(room, 0, sizeof(*room));
    status = banana_identifier_copy(&room->room_id, room_id);
    if (status != BANANA_OK) {
        return status;
    }

    room->location = location;
    room->target_temp_c = target_temp_c;
    room->target_ethylene = target_ethylene;
    room->target_stage = target_stage;
    return BANANA_OK;
}

BananaStatus banana_ripening_room_admit_batch(
    BananaRipeningRoom* room,
    const BananaBatch* batch
) {
    BananaStatus status = BANANA_OK;

    if (room == 0 || batch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (room->batch_count >= BANANA_MAX_BATCHES_PER_RIPENING_ROOM
        || identifier_index(room->batch_ids, room->batch_count, batch->batch_id.value) >= 0) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&room->batch_ids[room->batch_count], batch->batch_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    room->batch_count++;
    return BANANA_OK;
}

BananaStatus banana_ripening_room_release_batch(
    BananaRipeningRoom* room,
    const char* batch_id
) {
    int index = -1;
    int current = 0;

    if (room == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = identifier_index(room->batch_ids, room->batch_count, batch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    for (current = index; current < room->batch_count - 1; current++) {
        room->batch_ids[current] = room->batch_ids[current + 1];
    }

    memset(&room->batch_ids[room->batch_count - 1], 0, sizeof(BananaIdentifier));
    room->batch_count--;
    return BANANA_OK;
}

BananaStatus banana_ripening_room_apply(
    BananaRipeningRoom* room,
    BananaBatch* batch,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    BananaRipenessPrediction* prediction,
    BananaDomainEvent* event
) {
    BananaRipenessInput input;
    BananaStatus status = BANANA_OK;

    if (room == 0
        || batch == 0
        || prediction == 0
        || identifier_index(room->batch_ids, room->batch_count, batch->batch_id.value) < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    input.temperature_history_c = temperature_history_c;
    input.temperature_history_count = temperature_history_count;
    input.days_since_harvest = days_since_harvest;
    input.ethylene_exposure = batch->ethylene_exposure + room->target_ethylene;
    input.mechanical_damage = 0.0;
    input.storage_temp_c = room->target_temp_c;

    status = banana_predict_ripeness(&input, prediction);
    if (status != BANANA_OK) {
        return status;
    }

    batch->storage_temp_c = room->target_temp_c;
    batch->ethylene_exposure = room->target_ethylene;
    return banana_domain_event_record(
        event,
        BANANA_EVENT_RIPENED,
        batch->batch_id.value,
        room->room_id.value,
        prediction->predicted_stage,
        batch->bunch_count,
        days_since_harvest,
        0.0);
}

BananaStatus banana_truck_register(
    const char* truck_id,
    BananaCurrentLocation location,
    double capacity_kg,
    BananaTruck* truck
) {
    BananaStatus status = BANANA_OK;

    if (truck == 0 || capacity_kg <= 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(truck, 0, sizeof(*truck));
    status = banana_identifier_copy(&truck->truck_id, truck_id);
    if (status != BANANA_OK) {
        return status;
    }

    truck->current_location = location;
    truck->capacity_kg = capacity_kg;
    return BANANA_OK;
}

BananaStatus banana_truck_load_container(
    BananaTruck* truck,
    const BananaContainer* container
) {
    BananaStatus status = BANANA_OK;

    if (truck == 0 || container == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (truck->container_count >= BANANA_MAX_CONTAINERS_PER_TRUCK
        || identifier_index(truck->container_ids, truck->container_count, container->container_id.value) >= 0
        || truck->current_load_kg + container->current_weight_kg > truck->capacity_kg) {
        return BANANA_ERROR_OVERFLOW;
    }

    status = banana_identifier_copy(&truck->container_ids[truck->container_count], container->container_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    truck->container_count++;
    truck->current_load_kg += container->current_weight_kg;
    return BANANA_OK;
}

BananaStatus banana_truck_unload_container(
    BananaTruck* truck,
    const char* container_id,
    double container_weight_kg
) {
    int index = -1;
    int current = 0;

    if (truck == 0 || container_weight_kg < 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = identifier_index(truck->container_ids, truck->container_count, container_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    for (current = index; current < truck->container_count - 1; current++) {
        truck->container_ids[current] = truck->container_ids[current + 1];
    }

    memset(&truck->container_ids[truck->container_count - 1], 0, sizeof(BananaIdentifier));
    truck->container_count--;
    if (truck->current_load_kg >= container_weight_kg) {
        truck->current_load_kg -= container_weight_kg;
    } else {
        truck->current_load_kg = 0.0;
    }

    return BANANA_OK;
}

BananaStatus banana_truck_relocate(
    BananaTruck* truck,
    BananaCurrentLocation location
) {
    if (truck == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    truck->current_location = location;
    return BANANA_OK;
}