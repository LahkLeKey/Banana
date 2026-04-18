#include "../banana_wrapper_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/domain/banana_lifecycle.h"
#include "../../core/domain/banana_logistics.h"

#define BANANA_MAX_WRAPPER_TRUCKS 64

static BananaTruck truck_registry[BANANA_MAX_WRAPPER_TRUCKS];
static int truck_registry_count = 0;

static const char* distribution_node_type_name(DistributionNodeType type) {
    switch (type) {
        case BANANA_NODE_PORT:
            return "PORT";
        case BANANA_NODE_WAREHOUSE:
            return "WAREHOUSE";
        case BANANA_NODE_RETAIL:
            return "RETAIL";
        case BANANA_NODE_RIPENING_CENTER:
            return "RIPENING_CENTER";
    }

    return "UNKNOWN";
}

static int try_map_distribution_node_type(CInteropDistributionNodeType raw_value, DistributionNodeType* type) {
    if (type == 0) {
        return 0;
    }

    switch (raw_value) {
        case BANANA_NODE_PORT:
        case BANANA_NODE_WAREHOUSE:
        case BANANA_NODE_RETAIL:
        case BANANA_NODE_RIPENING_CENTER:
            *type = (DistributionNodeType)raw_value;
            return 1;
        default:
            return 0;
    }
}

static int find_truck_index(const char* truck_id) {
    int index = 0;

    if (!banana_identifier_is_valid(truck_id)) {
        return -1;
    }

    for (index = 0; index < truck_registry_count; index++) {
        if (strcmp(truck_registry[index].truck_id.value, truck_id) == 0) {
            return index;
        }
    }

    return -1;
}

static BananaStatus save_truck(const BananaTruck* truck) {
    int index = -1;

    if (truck == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = find_truck_index(truck->truck_id.value);
    if (index >= 0) {
        truck_registry[index] = *truck;
        return BANANA_OK;
    }

    if (truck_registry_count >= BANANA_MAX_WRAPPER_TRUCKS) {
        return BANANA_ERROR_OVERFLOW;
    }

    truck_registry[truck_registry_count] = *truck;
    truck_registry_count++;
    return BANANA_OK;
}

static BananaStatus load_truck(const char* truck_id, BananaTruck* truck) {
    int index = -1;

    if (truck == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = find_truck_index(truck_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *truck = truck_registry[index];
    return BANANA_OK;
}

static BananaStatus create_container_snapshot(
    const char* container_id,
    double container_weight_kg,
    BananaContainer* container
) {
    BananaStatus status = BANANA_OK;

    if (container == 0 || container_weight_kg < 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(container, 0, sizeof(*container));
    status = banana_identifier_copy(&container->container_id, container_id);
    if (status != BANANA_OK) {
        return status;
    }

    container->current_weight_kg = container_weight_kg;
    return BANANA_OK;
}

static char* create_truck_json(const BananaTruck* truck) {
    int required = 0;
    char* payload = 0;

    if (truck == 0) {
        return 0;
    }

    required = snprintf(
        0,
        0,
        "{\"truckId\":\"%s\",\"currentNodeId\":\"%s\",\"nodeType\":\"%s\",\"latitude\":%.4f,\"longitude\":%.4f,\"capacityKg\":%.2f,\"currentLoadKg\":%.2f,\"containerCount\":%d}",
        truck->truck_id.value,
        truck->current_location.node_id.value,
        distribution_node_type_name(truck->current_location.node_type),
        truck->current_location.coordinates.latitude,
        truck->current_location.coordinates.longitude,
        truck->capacity_kg,
        truck->current_load_kg,
        truck->container_count);
    if (required < 0) {
        return 0;
    }

    payload = (char*)malloc((size_t)required + 1U);
    if (payload == 0) {
        return 0;
    }

    if (snprintf(
        payload,
        (size_t)required + 1U,
        "{\"truckId\":\"%s\",\"currentNodeId\":\"%s\",\"nodeType\":\"%s\",\"latitude\":%.4f,\"longitude\":%.4f,\"capacityKg\":%.2f,\"currentLoadKg\":%.2f,\"containerCount\":%d}",
        truck->truck_id.value,
        truck->current_location.node_id.value,
        distribution_node_type_name(truck->current_location.node_type),
        truck->current_location.coordinates.latitude,
        truck->current_location.coordinates.longitude,
        truck->capacity_kg,
        truck->current_load_kg,
        truck->container_count) < 0) {
        free(payload);
        return 0;
    }

    return payload;
}

int banana_register_truck(
    const char* truck_id,
    const char* node_id,
    CInteropDistributionNodeType node_type,
    double latitude,
    double longitude,
    double capacity_kg,
    char** out_truck_json
) {
    BananaTruck truck;
    BananaGeoCoordinates coordinates;
    BananaCurrentLocation location;
    DistributionNodeType mapped_node_type;
    BananaStatus status = BANANA_OK;

    if (out_truck_json == 0 || !try_map_distribution_node_type(node_type, &mapped_node_type)) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_truck_json = 0;
    coordinates.latitude = latitude;
    coordinates.longitude = longitude;

    status = banana_current_location_set(node_id, coordinates, mapped_node_type, &location);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = banana_truck_register(truck_id, location, capacity_kg, &truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = save_truck(&truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_truck_json = create_truck_json(&truck);
    if (*out_truck_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_load_truck_container(
    const char* truck_id,
    const char* container_id,
    double container_weight_kg,
    char** out_truck_json
) {
    BananaTruck truck;
    BananaContainer container;
    BananaStatus status = BANANA_OK;

    if (out_truck_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_truck_json = 0;
    status = load_truck(truck_id, &truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = create_container_snapshot(container_id, container_weight_kg, &container);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = banana_truck_load_container(&truck, &container);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = save_truck(&truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_truck_json = create_truck_json(&truck);
    if (*out_truck_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_unload_truck_container(
    const char* truck_id,
    const char* container_id,
    double container_weight_kg,
    char** out_truck_json
) {
    BananaTruck truck;
    BananaStatus status = BANANA_OK;

    if (out_truck_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_truck_json = 0;
    status = load_truck(truck_id, &truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = banana_truck_unload_container(&truck, container_id, container_weight_kg);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = save_truck(&truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_truck_json = create_truck_json(&truck);
    if (*out_truck_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_relocate_truck(
    const char* truck_id,
    const char* node_id,
    CInteropDistributionNodeType node_type,
    double latitude,
    double longitude,
    char** out_truck_json
) {
    BananaTruck truck;
    BananaGeoCoordinates coordinates;
    BananaCurrentLocation location;
    DistributionNodeType mapped_node_type;
    BananaStatus status = BANANA_OK;

    if (out_truck_json == 0 || !try_map_distribution_node_type(node_type, &mapped_node_type)) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_truck_json = 0;
    status = load_truck(truck_id, &truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    coordinates.latitude = latitude;
    coordinates.longitude = longitude;
    status = banana_current_location_set(node_id, coordinates, mapped_node_type, &location);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = banana_truck_relocate(&truck, location);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = save_truck(&truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_truck_json = create_truck_json(&truck);
    if (*out_truck_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}

int banana_get_truck_status(
    const char* truck_id,
    char** out_truck_json
) {
    BananaTruck truck;
    BananaStatus status = BANANA_OK;

    if (out_truck_json == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_truck_json = 0;
    status = load_truck(truck_id, &truck);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    *out_truck_json = create_truck_json(&truck);
    if (*out_truck_json == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    return BANANA_STATUS_OK;
}