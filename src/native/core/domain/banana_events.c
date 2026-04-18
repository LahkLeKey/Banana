#include "banana_events.h"

#include <string.h>

static BananaStatus copy_optional_identifier(BananaIdentifier* identifier, const char* value) {
    if (identifier == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(identifier->value, 0, sizeof(identifier->value));
    if (value == 0 || value[0] == '\0') {
        return BANANA_OK;
    }

    return banana_identifier_copy(identifier, value);
}

const char* banana_domain_event_type_name(BananaDomainEventType type) {
    switch (type) {
        case BANANA_EVENT_PLANTED:
            return "PLANTED";
        case BANANA_EVENT_HARVESTED:
            return "HARVESTED";
        case BANANA_EVENT_SHIPPED:
            return "SHIPPED";
        case BANANA_EVENT_ARRIVED:
            return "ARRIVED";
        case BANANA_EVENT_RIPENED:
            return "RIPENED";
        case BANANA_EVENT_SOLD:
            return "SOLD";
        case BANANA_EVENT_SPOILED:
            return "SPOILED";
        case BANANA_EVENT_INVENTORY_RECEIVED:
            return "INVENTORY_RECEIVED";
        case BANANA_EVENT_NONE:
        default:
            return "NONE";
    }
}

void banana_domain_event_clear(BananaDomainEvent* event) {
    if (event == 0) {
        return;
    }

    memset(event, 0, sizeof(*event));
}

BananaStatus banana_domain_event_record(
    BananaDomainEvent* event,
    BananaDomainEventType type,
    const char* aggregate_id,
    const char* related_id,
    BananaRipenessStage ripeness_stage,
    int quantity,
    int event_day_ordinal,
    double weight_kg
) {
    BananaStatus status = BANANA_OK;

    if (event == 0) {
        return BANANA_OK;
    }

    banana_domain_event_clear(event);
    event->type = type;
    event->ripeness_stage = ripeness_stage;
    event->quantity = quantity;
    event->event_day_ordinal = event_day_ordinal;
    event->weight_kg = weight_kg;

    status = copy_optional_identifier(&event->aggregate_id, aggregate_id);
    if (status != BANANA_OK) {
        banana_domain_event_clear(event);
        return status;
    }

    status = copy_optional_identifier(&event->related_id, related_id);
    if (status != BANANA_OK) {
        banana_domain_event_clear(event);
        return status;
    }

    return BANANA_OK;
}