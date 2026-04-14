#ifndef BANANA_H
#define BANANA_H

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

typedef struct BananaRules {
    int base_rate;
    int bonus_threshold;
    int bonus_rate;
} BananaRules;

typedef enum BananaStatus {
    BANANA_OK = 0,
    BANANA_ERROR_INVALID_INPUT = 1,
    BANANA_ERROR_OVERFLOW = 2
} BananaStatus;

typedef void (*BananaStep)(BananaResult* result);

typedef struct BananaPipeline {
    BananaStep* steps;
    int count;
} BananaPipeline;

BananaRules banana_default_rules(void);

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

#ifdef __cplusplus
}
#endif

#endif
