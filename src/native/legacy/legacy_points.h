#ifndef LEGACY_POINTS_H
#define LEGACY_POINTS_H

typedef struct LegacyPointsSummary {
    int purchases;
    int multiplier;
    int base_points;
    int bonus_points;
    int total_points;
} LegacyPointsSummary;

int legacy_calculate_points(int purchases, int multiplier);
int legacy_calculate_summary(int purchases, int multiplier, LegacyPointsSummary* summary);

#endif
