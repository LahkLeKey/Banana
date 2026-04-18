#ifndef BANANA_INVENTORY_H
#define BANANA_INVENTORY_H

#include "banana_events.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BananaInventoryItem {
    BananaIdentifier inventory_id;
    BananaIdentifier location_id;
    BananaIdentifier source_batch_id;
    BananaRipenessStage ripeness_stage;
    int quantity_on_hand;
    int reorder_threshold;
    double total_weight_kg;
} BananaInventoryItem;

BananaStatus banana_inventory_receive(
    const char* inventory_id,
    const char* location_id,
    const char* source_batch_id,
    BananaRipenessStage ripeness_stage,
    int quantity_on_hand,
    int reorder_threshold,
    double total_weight_kg,
    int event_day_ordinal,
    BananaInventoryItem* item,
    BananaDomainEvent* event
);

BananaStatus banana_inventory_sell(
    BananaInventoryItem* item,
    int quantity,
    int event_day_ordinal,
    BananaDomainEvent* event
);

BananaStatus banana_inventory_discard_spoiled(
    BananaInventoryItem* item,
    int quantity,
    int event_day_ordinal,
    BananaDomainEvent* event
);

int banana_inventory_requires_reorder(const BananaInventoryItem* item);

#ifdef __cplusplus
}
#endif

#endif