#ifndef BANANA_CULTIVATION_H
#define BANANA_CULTIVATION_H

#include "banana_events.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BananaBunchLifecycleState {
    BANANA_BUNCH_STATE_PLANTED = 0,
    BANANA_BUNCH_STATE_GROWING = 1,
    BANANA_BUNCH_STATE_HARVESTED = 2,
    BANANA_BUNCH_STATE_RIPENING = 3,
    BANANA_BUNCH_STATE_SOLD = 4,
    BANANA_BUNCH_STATE_SPOILED = 5
} BananaBunchLifecycleState;

typedef struct BananaBunchSpec {
    int harvest_day_ordinal;
    double weight_kg;
    int finger_count;
    BananaRipenessStage maturity_stage;
    double quality_score;
} BananaBunchSpec;

typedef struct BananaBunchAggregate {
    BananaBunch bunch;
    BananaBunchLifecycleState lifecycle_state;
} BananaBunchAggregate;

BananaStatus banana_plant_register(
    const char* plant_id,
    BananaSpecies species,
    BananaGeoCoordinates location,
    int planted_day_ordinal,
    int pseudostem_count,
    BananaPlant* plant,
    BananaDomainEvent* event
);

BananaStatus banana_plant_add_sucker(
    BananaPlant* plant,
    const char* sucker_id
);

BananaStatus banana_plant_record_bloom(
    const BananaPlant* plant,
    const char* bunch_id,
    int bloom_day_ordinal,
    BananaDomainEvent* event
);

BananaStatus banana_bunch_harvest(
    const BananaPlant* plant,
    const char* bunch_id,
    const BananaBunchSpec* spec,
    BananaBunchAggregate* bunch,
    BananaDomainEvent* event
);

BananaStatus banana_bunch_update_ripeness(
    BananaBunchAggregate* bunch,
    BananaRipenessStage new_stage,
    int event_day_ordinal,
    BananaDomainEvent* event
);

BananaStatus banana_bunch_mark_spoiled(
    BananaBunchAggregate* bunch,
    int event_day_ordinal,
    BananaDomainEvent* event
);

int banana_bunch_is_market_ready(const BananaBunchAggregate* bunch);

#ifdef __cplusplus
}
#endif

#endif