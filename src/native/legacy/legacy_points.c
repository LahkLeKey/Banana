#include "legacy_points.h"

int legacy_calculate_points(int purchases, int multiplier) {
    int base_points = purchases * 10;
    int bonus_points = (purchases >= 10) ? (multiplier * 25) : 0;
    return base_points + bonus_points;
}

int legacy_calculate_summary(int purchases, int multiplier, LegacyPointsSummary* summary) {
    if (summary == 0) {
        return -1;
    }

    summary->purchases = purchases;
    summary->multiplier = multiplier;
    summary->base_points = purchases * 10;
    summary->bonus_points = (purchases >= 10) ? (multiplier * 25) : 0;
    summary->total_points = summary->base_points + summary->bonus_points;

    return 0;
}
