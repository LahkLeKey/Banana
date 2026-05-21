#include "anticheat_synthetic.h"
void anticheat_synthetic_inject(const char *session_id, int event_type, int param) {
    (void)session_id; (void)event_type; (void)param;
    extern void anticheat_metrics_increment(int metric_id);
    anticheat_metrics_increment(12); // anomaly_events
}
