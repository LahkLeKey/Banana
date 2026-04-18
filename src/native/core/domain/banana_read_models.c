#include "banana_read_models.h"

#include <string.h>

static int stage_index(BananaRipenessStage stage) {
    if (stage < BANANA_STAGE_GREEN || stage > BANANA_STAGE_BIODEGRADATION) {
        return -1;
    }

    return (int)stage;
}

void banana_stock_view_clear(BananaStockView* view) {
    if (view == 0) {
        return;
    }

    memset(view, 0, sizeof(*view));
}

BananaStatus banana_stock_view_project_inventory_event(
    BananaStockView* view,
    const BananaInventoryItem* item,
    const BananaDomainEvent* event
) {
    int index = 0;

    if (view == 0 || item == 0 || event == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (view->location_id.value[0] == '\0') {
        view->location_id = item->location_id;
    }

    if (strcmp(view->location_id.value, item->location_id.value) != 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(view->stage_quantities, 0, sizeof(view->stage_quantities));
    index = stage_index(item->ripeness_stage);
    if (index < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    view->stage_quantities[index] = item->quantity_on_hand;
    view->total_quantity = item->quantity_on_hand;
    view->total_weight_kg = item->total_weight_kg;
    return BANANA_OK;
}

void banana_average_ripeness_report_clear(BananaAverageRipenessReport* report) {
    if (report == 0) {
        return;
    }

    memset(report, 0, sizeof(*report));
}

BananaStatus banana_average_ripeness_report_record_prediction(
    BananaAverageRipenessReport* report,
    const BananaRipenessPrediction* prediction
) {
    int index = -1;

    if (report == 0 || prediction == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    index = stage_index(prediction->predicted_stage);
    if (index < 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    report->observation_count++;
    report->total_stage_score += (double)index;
    report->average_stage_score = report->total_stage_score / (double)report->observation_count;
    report->stage_observations[index]++;
    return BANANA_OK;
}

BananaStatus banana_average_ripeness_report_record_bunch(
    BananaAverageRipenessReport* report,
    const BananaBunchRecord* bunch
) {
    BananaRipenessPrediction prediction;

    if (report == 0 || bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(&prediction, 0, sizeof(prediction));
    prediction.predicted_stage = bunch->aggregate.bunch.maturity_stage;
    return banana_average_ripeness_report_record_prediction(report, &prediction);
}

void banana_cultivar_stats_clear(BananaCultivarStats* stats, BananaSpecies cultivar) {
    if (stats == 0) {
        return;
    }

    memset(stats, 0, sizeof(*stats));
    stats->cultivar = cultivar;
}

BananaStatus banana_cultivar_stats_record_harvest(
    BananaCultivarStats* stats,
    const BananaBunchRecord* bunch
) {
    if (stats == 0 || bunch == 0 || bunch->cultivar != stats->cultivar) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    stats->harvested_bunch_count++;
    stats->harvested_fruit_count += bunch->banana_count;
    stats->harvested_weight_kg += bunch->total_weight_kg;
    return BANANA_OK;
}

BananaStatus banana_cultivar_stats_record_sale(
    BananaCultivarStats* stats,
    int sold_quantity,
    double sold_weight_kg
) {
    if (stats == 0 || sold_quantity < 0 || sold_weight_kg < 0.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    stats->sold_fruit_count += sold_quantity;
    stats->sold_weight_kg += sold_weight_kg;
    return BANANA_OK;
}