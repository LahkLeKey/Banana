#ifndef BANANA_LOGISTICS_H
#define BANANA_LOGISTICS_H

#include "banana_supply_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_BATCHES_PER_CONTAINER 8
#define BANANA_MAX_BATCHES_PER_RIPENING_ROOM 16
#define BANANA_MAX_CONTAINERS_PER_TRUCK 8

typedef struct BananaTemperatureSetting {
    double set_point_c;
    double tolerance_c;
} BananaTemperatureSetting;

typedef struct BananaCurrentLocation {
    BananaIdentifier node_id;
    BananaGeoCoordinates coordinates;
    DistributionNodeType node_type;
} BananaCurrentLocation;

typedef struct BananaContainer {
    BananaIdentifier container_id;
    BananaCurrentLocation current_location;
    BananaTemperatureSetting temperature;
    double capacity_kg;
    double current_weight_kg;
    int batch_count;
    BananaIdentifier batch_ids[BANANA_MAX_BATCHES_PER_CONTAINER];
} BananaContainer;

typedef struct BananaRipeningRoom {
    BananaIdentifier room_id;
    BananaCurrentLocation location;
    double target_temp_c;
    double target_ethylene;
    BananaRipenessStage target_stage;
    int batch_count;
    BananaIdentifier batch_ids[BANANA_MAX_BATCHES_PER_RIPENING_ROOM];
} BananaRipeningRoom;

typedef struct BananaTruck {
    BananaIdentifier truck_id;
    BananaCurrentLocation current_location;
    double capacity_kg;
    double current_load_kg;
    int container_count;
    BananaIdentifier container_ids[BANANA_MAX_CONTAINERS_PER_TRUCK];
} BananaTruck;

BananaStatus banana_temperature_setting_validate(const BananaTemperatureSetting* setting);

BananaStatus banana_current_location_set(
    const char* node_id,
    BananaGeoCoordinates coordinates,
    DistributionNodeType node_type,
    BananaCurrentLocation* location
);

BananaStatus banana_container_register(
    const char* container_id,
    BananaCurrentLocation location,
    BananaTemperatureSetting temperature,
    double capacity_kg,
    BananaContainer* container
);

BananaStatus banana_container_load_batch(
    BananaContainer* container,
    const BananaBatch* batch
);

BananaStatus banana_container_unload_batch(
    BananaContainer* container,
    const char* batch_id
);

BananaStatus banana_ripening_room_register(
    const char* room_id,
    BananaCurrentLocation location,
    double target_temp_c,
    double target_ethylene,
    BananaRipenessStage target_stage,
    BananaRipeningRoom* room
);

BananaStatus banana_ripening_room_admit_batch(
    BananaRipeningRoom* room,
    const BananaBatch* batch
);

BananaStatus banana_ripening_room_release_batch(
    BananaRipeningRoom* room,
    const char* batch_id
);

BananaStatus banana_ripening_room_apply(
    BananaRipeningRoom* room,
    BananaBatch* batch,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    BananaRipenessPrediction* prediction,
    BananaDomainEvent* event
);

BananaStatus banana_truck_register(
    const char* truck_id,
    BananaCurrentLocation location,
    double capacity_kg,
    BananaTruck* truck
);

BananaStatus banana_truck_load_container(
    BananaTruck* truck,
    const BananaContainer* container
);

BananaStatus banana_truck_unload_container(
    BananaTruck* truck,
    const char* container_id,
    double container_weight_kg
);

BananaStatus banana_truck_relocate(
    BananaTruck* truck,
    BananaCurrentLocation location
);

#ifdef __cplusplus
}
#endif

#endif