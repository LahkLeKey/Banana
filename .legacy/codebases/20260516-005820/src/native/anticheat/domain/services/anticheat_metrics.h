// Metrics domain
#ifndef BANANA_ANTICHEAT_METRICS_H
#define BANANA_ANTICHEAT_METRICS_H
#include "banana_anticheat.h"
void anticheat_metrics_increment(int metric_id);
void anticheat_metrics_get(BananaAnticheatMetrics *metrics);
#endif