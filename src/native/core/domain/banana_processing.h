#ifndef BANANA_PROCESSING_H
#define BANANA_PROCESSING_H

#include "banana_cultivation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_FRUIT_PER_BUNCH 256

typedef struct BananaFruit {
    BananaIdentifier fruit_id;
    BananaSpecies cultivar;
    BananaRipenessStage ripeness_stage;
    double weight_kg;
} BananaFruit;

typedef struct BananaFruitSpec {
    const char* fruit_id;
    BananaSpecies cultivar;
    BananaRipenessStage ripeness_stage;
    double weight_kg;
} BananaFruitSpec;

typedef struct BananaBunchRecord {
    BananaBunchAggregate aggregate;
    BananaSpecies cultivar;
    int banana_count;
    BananaFruit bananas[BANANA_MAX_FRUIT_PER_BUNCH];
    double total_weight_kg;
} BananaBunchRecord;

BananaStatus banana_bunch_factory_create(
    const BananaPlant* plant,
    const char* bunch_id,
    const BananaBunchSpec* spec,
    BananaSpecies cultivar,
    const BananaFruitSpec* fruit_specs,
    int fruit_count,
    BananaBunchRecord* record,
    BananaDomainEvent* event
);

BananaStatus banana_bunch_record_validate(const BananaBunchRecord* record);

BananaStatus banana_bunch_record_update_ripeness(
    BananaBunchRecord* record,
    BananaRipenessStage new_stage,
    int event_day_ordinal,
    BananaDomainEvent* event
);

double banana_bunch_record_total_weight_kg(const BananaBunchRecord* record);

int banana_bunch_record_all_same_cultivar(const BananaBunchRecord* record);

int banana_bunch_record_uniform_ripeness(const BananaBunchRecord* record);

#ifdef __cplusplus
}
#endif

#endif