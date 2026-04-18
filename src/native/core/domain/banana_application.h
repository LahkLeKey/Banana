#ifndef BANANA_APPLICATION_H
#define BANANA_APPLICATION_H

#include "banana_integration.h"
#include "banana_repositories.h"
#include "banana_services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BananaHarvestCommand {
    const char* bunch_id;
    int bloom_day_ordinal;
    BananaBunchSpec bunch_spec;
    BananaSpecies cultivar;
    const BananaFruitSpec* fruit_specs;
    int fruit_count;
} BananaHarvestCommand;

typedef struct BananaRipenCommand {
    const char* bunch_id;
    BananaRipenessInput input;
    int event_day_ordinal;
} BananaRipenCommand;

typedef struct BananaShipCommand {
    const char* bunch_id;
    const char* batch_id;
    const char* origin_farm;
    double storage_temp_c;
    double ethylene_exposure;
    int estimated_shelf_life_days;
    const char* shipment_id;
    const char* origin_node_id;
    const char* destination_node_id;
    int departure_day_ordinal;
} BananaShipCommand;

typedef struct BananaInventoryReceiveCommand {
    const char* inventory_id;
    const char* location_id;
    const char* source_batch_id;
    BananaRipenessStage ripeness_stage;
    int quantity_on_hand;
    int reorder_threshold;
    double total_weight_kg;
    int event_day_ordinal;
} BananaInventoryReceiveCommand;

typedef struct BananaSellCommand {
    const char* inventory_id;
    int quantity;
    int event_day_ordinal;
} BananaSellCommand;

typedef struct BananaDiscardSpoiledCommand {
    const char* inventory_id;
    int quantity;
    int event_day_ordinal;
} BananaDiscardSpoiledCommand;

BananaStatus banana_application_harvest_bunch(
    const char* plant_id,
    const BananaHarvestCommand* command,
    BananaBunchRecord* bunch,
    BananaDomainEvent* bloom_event,
    BananaDomainEvent* harvest_event
);

BananaStatus banana_application_ripen_bunch(
    const BananaRipenCommand* command,
    BananaBunchRecord* bunch,
    BananaRipenessPrediction* prediction,
    BananaDomainEvent* event
);

BananaStatus banana_application_ship_bunch(
    const BananaShipCommand* command,
    BananaBatch* batch,
    BananaShipment* shipment,
    BananaDomainEvent* event
);

BananaStatus banana_application_receive_inventory(
    const BananaInventoryReceiveCommand* command,
    BananaInventoryItem* item,
    BananaDomainEvent* event
);

BananaStatus banana_application_sell_bananas(
    const BananaSellCommand* command,
    BananaInventoryItem* item,
    BananaDomainEvent* event
);

BananaStatus banana_application_discard_spoiled(
    const BananaDiscardSpoiledCommand* command,
    BananaInventoryItem* item,
    BananaDomainEvent* event
);

#ifdef __cplusplus
}
#endif

#endif