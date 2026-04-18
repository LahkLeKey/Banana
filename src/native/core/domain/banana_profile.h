#ifndef BANANA_PROFILE_H
#define BANANA_PROFILE_H

#include "banana_lifecycle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BananaInput {
    int purchases;
    int multiplier;
} BananaInput;

typedef struct BananaResult {
    int banana;
} BananaResult;

typedef struct BananaCalculationBreakdown {
    int base_banana;
    int bonus_banana;
} BananaCalculationBreakdown;

typedef struct BananaRules {
    int base_rate;
    int bonus_threshold;
    int bonus_rate;
} BananaRules;

typedef void (*BananaStep)(BananaResult* result);

typedef struct BananaPipeline {
    BananaStep* steps;
    int count;
} BananaPipeline;

typedef struct BananaExecutionContext {
    BananaInput input;
    BananaRules rules;
    BananaResult result;
    BananaCalculationBreakdown breakdown;
    BananaStatus status;
} BananaExecutionContext;

BananaRules banana_default_rules(void);

BananaStatus banana_validate_input(
    const BananaInput* input,
    const BananaRules* rules
);

BananaStatus banana_prepare_execution_context(
    const BananaInput* input,
    const BananaRules* rules,
    BananaExecutionContext* context
);

BananaStatus banana_calculate_context(
    BananaExecutionContext* context
);

BananaStatus banana_calculate(
    const BananaInput* input,
    BananaResult* output
);

BananaStatus banana_calculate_with_rules(
    const BananaInput* input,
    const BananaRules* rules,
    BananaResult* output
);

void banana_run_pipeline(
    BananaResult* result,
    const BananaPipeline* pipeline
);

BananaStatus banana_predict_ripeness_for_profile_input(
    const BananaInput* input,
    BananaRipenessPrediction* prediction
);

#ifdef __cplusplus
}
#endif

#endif