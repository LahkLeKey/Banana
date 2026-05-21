#include "anticheat_metrics.h"
static BananaAnticheatMetrics g_metrics = {0, {0}, 0, 0, 0, 0, 0, 0, 0};
void anticheat_metrics_increment(int metric_id) {
    if (metric_id == 0) g_metrics.active_sessions++;
    else if (metric_id >= 1 && metric_id <= 7) g_metrics.trust_tier_counts[metric_id-1]++;
    else if (metric_id == 8) g_metrics.replay_rejections++;
    else if (metric_id == 9) g_metrics.challenge_successes++;
    else if (metric_id == 10) g_metrics.challenge_failures++;
    else if (metric_id == 11) g_metrics.telemetry_parse_failures++;
    else if (metric_id == 12) g_metrics.anomaly_events++;
    else if (metric_id == 13) g_metrics.enforcement_actions++;
    else if (metric_id == 14) g_metrics.behavioral_validation_hits++;
    else (void)metric_id;
}
void anticheat_metrics_get(BananaAnticheatMetrics *metrics) {
    if (metrics) *metrics = g_metrics;
}
