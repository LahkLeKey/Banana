#ifndef BANANA_READ_MODELS_H
#define BANANA_READ_MODELS_H

#include "banana_processing.h"
#include "banana_inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BananaStockView {
    BananaIdentifier location_id;
    int stage_quantities[6];
    int total_quantity;
    double total_weight_kg;
} BananaStockView;

typedef struct BananaAverageRipenessReport {
    int observation_count;
    double total_stage_score;
    double average_stage_score;
    int stage_observations[6];
} BananaAverageRipenessReport;

typedef struct BananaCultivarStats {
    BananaSpecies cultivar;
    int harvested_bunch_count;
    int harvested_fruit_count;
    double harvested_weight_kg;
    int sold_fruit_count;
    double sold_weight_kg;
} BananaCultivarStats;

void banana_stock_view_clear(BananaStockView* view);

BananaStatus banana_stock_view_project_inventory_event(
    BananaStockView* view,
    const BananaInventoryItem* item,
    const BananaDomainEvent* event
);

void banana_average_ripeness_report_clear(BananaAverageRipenessReport* report);

BananaStatus banana_average_ripeness_report_record_prediction(
    BananaAverageRipenessReport* report,
    const BananaRipenessPrediction* prediction
);

BananaStatus banana_average_ripeness_report_record_bunch(
    BananaAverageRipenessReport* report,
    const BananaBunchRecord* bunch
);

void banana_cultivar_stats_clear(BananaCultivarStats* stats, BananaSpecies cultivar);

BananaStatus banana_cultivar_stats_record_harvest(
    BananaCultivarStats* stats,
    const BananaBunchRecord* bunch
);

BananaStatus banana_cultivar_stats_record_sale(
    BananaCultivarStats* stats,
    int sold_quantity,
    double sold_weight_kg
);

#ifdef __cplusplus
}
#endif

#endif