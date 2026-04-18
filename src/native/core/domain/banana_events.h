#ifndef BANANA_EVENTS_H
#define BANANA_EVENTS_H

#include "banana_lifecycle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BananaDomainEventType {
    BANANA_EVENT_NONE = 0,
    BANANA_EVENT_PLANTED = 1,
    BANANA_EVENT_BLOOMED = 2,
    BANANA_EVENT_HARVESTED = 3,
    BANANA_EVENT_SHIPPED = 4,
    BANANA_EVENT_ARRIVED = 5,
    BANANA_EVENT_RIPENED = 6,
    BANANA_EVENT_SOLD = 7,
    BANANA_EVENT_SPOILED = 8,
    BANANA_EVENT_INVENTORY_RECEIVED = 9
} BananaDomainEventType;

typedef struct BananaDomainEvent {
    BananaDomainEventType type;
    BananaIdentifier aggregate_id;
    BananaIdentifier related_id;
    BananaRipenessStage ripeness_stage;
    int quantity;
    int event_day_ordinal;
    double weight_kg;
} BananaDomainEvent;

const char* banana_domain_event_type_name(BananaDomainEventType type);

void banana_domain_event_clear(BananaDomainEvent* event);

BananaStatus banana_domain_event_record(
    BananaDomainEvent* event,
    BananaDomainEventType type,
    const char* aggregate_id,
    const char* related_id,
    BananaRipenessStage ripeness_stage,
    int quantity,
    int event_day_ordinal,
    double weight_kg
);

#ifdef __cplusplus
}
#endif

#endif