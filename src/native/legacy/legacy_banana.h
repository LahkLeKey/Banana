#ifndef LEGACY_BANANA_H
#define LEGACY_BANANA_H

typedef struct LegacyBananaSummary {
    int purchases;
    int multiplier;
    int banana;
} LegacyBananaSummary;

int legacy_calculate_banana(int purchases, int multiplier);
int legacy_calculate_summary(int purchases, int multiplier, LegacyBananaSummary* summary);

#endif
