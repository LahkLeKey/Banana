#ifndef BANANA_SUPPLY_CHAIN_H
#define BANANA_SUPPLY_CHAIN_H

#include "banana_events.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_BUNCHES_PER_BATCH 16
#define BANANA_MAX_BATCHES_PER_NODE 32
#define BANANA_MAX_BATCH_REGISTRY 64
#define BANANA_MAX_BATCHES_PER_SHIPMENT 16

typedef enum BananaExportStatus {
    BANANA_EXPORT_PACKED = 0,
    BANANA_EXPORT_SHIPPED = 1,
    BANANA_EXPORT_CUSTOMS = 2,
    BANANA_EXPORT_DISTRIBUTED = 3
} BananaExportStatus;

typedef enum DistributionNodeType {
    BANANA_NODE_PORT = 0,
    BANANA_NODE_WAREHOUSE = 1,
    BANANA_NODE_RETAIL = 2,
    BANANA_NODE_RIPENING_CENTER = 3
} DistributionNodeType;

typedef enum BananaShipmentStatus {
    BANANA_SHIPMENT_PLANNED = 0,
    BANANA_SHIPMENT_IN_TRANSIT = 1,
    BANANA_SHIPMENT_ARRIVED = 2,
    BANANA_SHIPMENT_COMPLETED = 3
} BananaShipmentStatus;

typedef struct BananaBatch {
    BananaIdentifier batch_id;
    BananaIdentifier origin_farm;
    int bunch_count;
    BananaIdentifier bunch_ids[BANANA_MAX_BUNCHES_PER_BATCH];
    BananaExportStatus export_status;
    double storage_temp_c;
    double ethylene_exposure;
    int estimated_shelf_life_days;
} BananaBatch;

typedef struct DistributionNode {
    BananaIdentifier node_id;
    DistributionNodeType type;
    BananaGeoCoordinates location;
    double capacity_kg;
    int inventory_batch_count;
    BananaIdentifier inventory_batches[BANANA_MAX_BATCHES_PER_NODE];
} DistributionNode;

typedef struct BananaShipment {
    BananaIdentifier shipment_id;
    BananaIdentifier origin_node_id;
    BananaIdentifier destination_node_id;
    BananaShipmentStatus status;
    int batch_count;
    BananaIdentifier batch_ids[BANANA_MAX_BATCHES_PER_SHIPMENT];
    int departure_day_ordinal;
    int arrival_day_ordinal;
} BananaShipment;

BananaStatus banana_batch_register(
    const char* batch_id,
    const char* origin_farm,
    double storage_temp_c,
    double ethylene_exposure,
    int estimated_shelf_life_days,
    BananaBatch* batch
);

BananaStatus banana_batch_get(
    const char* batch_id,
    BananaBatch* batch
);

BananaStatus banana_batch_add_bunch(
    BananaBatch* batch,
    const char* bunch_id
);

BananaStatus banana_batch_advance_export_status(
    BananaBatch* batch,
    BananaExportStatus next_status
);

BananaStatus banana_distribution_node_register(
    const char* node_id,
    DistributionNodeType type,
    BananaGeoCoordinates location,
    double capacity_kg,
    DistributionNode* node
);

BananaStatus banana_distribution_node_receive_batch(
    DistributionNode* node,
    const char* batch_id
);

BananaStatus banana_distribution_node_dispatch_batch(
    DistributionNode* node,
    const char* batch_id
);

BananaStatus banana_shipment_create(
    const char* shipment_id,
    const char* origin_node_id,
    const char* destination_node_id,
    BananaShipment* shipment
);

BananaStatus banana_shipment_add_batch(
    BananaShipment* shipment,
    const BananaBatch* batch
);

BananaStatus banana_shipment_dispatch(
    BananaShipment* shipment,
    BananaBatch* batch,
    int departure_day_ordinal,
    BananaDomainEvent* event
);

BananaStatus banana_shipment_arrive(
    BananaShipment* shipment,
    BananaBatch* batch,
    int arrival_day_ordinal,
    BananaDomainEvent* event
);

BananaStatus banana_batch_predict_ripeness(
    const char* batch_id,
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double mechanical_damage,
    BananaRipenessPrediction* prediction
);

#ifdef __cplusplus
}
#endif

#endif