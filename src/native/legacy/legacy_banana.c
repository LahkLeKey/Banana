#include "legacy_banana.h"

#include "../core/banana.h"

int legacy_calculate_banana(int purchases, int multiplier) {
    BananaInput input;
    BananaResult result;

    input.purchases = purchases;
    input.multiplier = multiplier;

    if (banana_calculate(&input, &result) != BANANA_OK) {
        return -1;
    }

    return result.banana;
}

int legacy_calculate_summary(int purchases, int multiplier, LegacyBananaSummary* summary) {
    BananaInput input;
    BananaResult result;

    if (summary == 0) {
        return -1;
    }

    input.purchases = purchases;
    input.multiplier = multiplier;

    if (banana_calculate(&input, &result) != BANANA_OK) {
        return -1;
    }

    summary->purchases = purchases;
    summary->multiplier = multiplier;
    summary->banana = result.banana;

    return 0;
}
