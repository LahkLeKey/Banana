#ifndef BANANA_PROCESSING_H
#define BANANA_PROCESSING_H

#include "banana_cultivation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_FRUIT_PER_BUNCH 256
#define BANANA_MAX_BUNCHES_PER_CRATE 8

typedef struct BananaDimensions {
    double length_cm;
    double girth_cm;
} BananaDimensions;

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

typedef struct BananaCrate {
    BananaIdentifier crate_id;
    double capacity_kg;
    double current_weight_kg;
    int bunch_count;
    BananaIdentifier bunch_ids[BANANA_MAX_BUNCHES_PER_CRATE];
} BananaCrate;

typedef struct BananaInspection {
    BananaIdentifier inspector_id;
    BananaIdentifier bunch_id;
    int accepted;
    double quality_score;
    int defect_count;
} BananaInspection;

BananaStatus banana_dimensions_validate(const BananaDimensions* dimensions);

BananaStatus banana_fruit_estimate_dimensions(
    const BananaFruit* fruit,
    BananaDimensions* dimensions
);

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

BananaStatus banana_crate_register(
    const char* crate_id,
    double capacity_kg,
    BananaCrate* crate
);

BananaStatus banana_crate_pack_bunch(
    BananaCrate* crate,
    const BananaBunchRecord* bunch
);

BananaStatus banana_inspection_record(
    const char* inspector_id,
    const BananaBunchRecord* bunch,
    double minimum_quality_score,
    int defect_count,
    BananaInspection* inspection
);

double banana_bunch_record_total_weight_kg(const BananaBunchRecord* record);

int banana_bunch_record_all_same_cultivar(const BananaBunchRecord* record);

int banana_bunch_record_uniform_ripeness(const BananaBunchRecord* record);

#ifdef __cplusplus
}
#endif

#endif