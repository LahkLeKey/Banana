#ifndef BANANA_SERVICES_H
#define BANANA_SERVICES_H

#include "banana_processing.h"

#ifdef __cplusplus
extern "C" {
#endif

double banana_quality_control_average_fruit_weight_kg(const BananaBunchRecord* bunch);

int banana_quality_control_is_underweight(
    const BananaBunchRecord* bunch,
    double minimum_average_weight_kg
);

BananaStatus banana_ripening_service_evaluate_bunch(
    const BananaBunchRecord* bunch,
    const BananaRipenessInput* input,
    BananaRipenessPrediction* prediction
);

BananaStatus banana_ripening_service_apply(
    BananaBunchRecord* bunch,
    const BananaRipenessInput* input,
    int event_day_ordinal,
    BananaRipenessPrediction* prediction,
    BananaDomainEvent* event
);

#ifdef __cplusplus
}
#endif

#endif