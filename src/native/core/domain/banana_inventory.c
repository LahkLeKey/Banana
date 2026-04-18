#include "banana_inventory.h"

#include <string.h>

static int ripeness_stage_is_valid(BananaRipenessStage stage) {
    return stage >= BANANA_STAGE_GREEN && stage <= BANANA_STAGE_BIODEGRADATION;
}

static double proportional_weight(double total_weight_kg, int original_quantity, int quantity_delta) {
    if (original_quantity <= 0) {
        return 0.0;
    }

    return total_weight_kg * ((double)quantity_delta / (double)original_quantity);
}

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
) {
    BananaStatus status = BANANA_OK;

    if (item == 0
        || !ripeness_stage_is_valid(ripeness_stage)
        || quantity_on_hand <= 0
        || reorder_threshold < 0
        || total_weight_kg <= 0.0
        || event_day_ordinal < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(item, 0, sizeof(*item));

    status = banana_identifier_copy(&item->inventory_id, inventory_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&item->location_id, location_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&item->source_batch_id, source_batch_id);
    if (status != BANANA_OK) {
        return status;
    }

    item->ripeness_stage = ripeness_stage;
    item->quantity_on_hand = quantity_on_hand;
    item->reorder_threshold = reorder_threshold;
    item->total_weight_kg = total_weight_kg;

    return banana_domain_event_record(
        event,
        BANANA_EVENT_INVENTORY_RECEIVED,
        inventory_id,
        source_batch_id,
        ripeness_stage,
        quantity_on_hand,
        event_day_ordinal,
        total_weight_kg);
}

BananaStatus banana_inventory_sell(
    BananaInventoryItem* item,
    int quantity,
    int event_day_ordinal,
    BananaDomainEvent* event
) {
    double sold_weight = 0.0;

    if (item == 0
        || quantity <= 0
        || event_day_ordinal < 0
        || quantity > item->quantity_on_hand) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    sold_weight = proportional_weight(item->total_weight_kg, item->quantity_on_hand, quantity);
    item->quantity_on_hand -= quantity;
    item->total_weight_kg -= sold_weight;
    if (item->total_weight_kg < 0.0) {
        item->total_weight_kg = 0.0;
    }

    return banana_domain_event_record(
        event,
        BANANA_EVENT_SOLD,
        item->inventory_id.value,
        item->source_batch_id.value,
        item->ripeness_stage,
        quantity,
        event_day_ordinal,
        sold_weight);
}

BananaStatus banana_inventory_discard_spoiled(
    BananaInventoryItem* item,
    int quantity,
    int event_day_ordinal,
    BananaDomainEvent* event
) {
    double discarded_weight = 0.0;

    if (item == 0
        || quantity <= 0
        || event_day_ordinal < 0
        || quantity > item->quantity_on_hand
        || item->ripeness_stage < BANANA_STAGE_OVERRIPE) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    discarded_weight = proportional_weight(item->total_weight_kg, item->quantity_on_hand, quantity);
    item->quantity_on_hand -= quantity;
    item->total_weight_kg -= discarded_weight;
    if (item->total_weight_kg < 0.0) {
        item->total_weight_kg = 0.0;
    }

    return banana_domain_event_record(
        event,
        BANANA_EVENT_SPOILED,
        item->inventory_id.value,
        item->source_batch_id.value,
        item->ripeness_stage,
        quantity,
        event_day_ordinal,
        discarded_weight);
}

int banana_inventory_requires_reorder(const BananaInventoryItem* item) {
    if (item == 0) {
        return 0;
    }

    return item->quantity_on_hand <= item->reorder_threshold;
}