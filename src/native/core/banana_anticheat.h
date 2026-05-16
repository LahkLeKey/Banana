/*
 * banana_anticheat.h — Native anti-cheat core for server-authoritative validation.
 *
 * Design goals:
 * - Compute-efficient fixed-cost session scoring (no heap churn in hot path)
 * - Usermode-safe telemetry ingestion (no kernel hooks)
 * - Suspicious driver signal scoring (ring-1/ring-2 class heuristics from usermode)
 */

#ifndef BANANA_ANTICHEAT_H
#define BANANA_ANTICHEAT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_core_anticheat_reset_session(const char *session_id);

    int banana_core_anticheat_submit_usermode_heartbeat(const char *session_id,
                                                         uint32_t heartbeat_sequence,
                                                         uint32_t simulation_tick,
                                                         int debugger_present,
                                                         int code_integrity_violations,
                                                         int memory_map_anomalies,
                                                         int timing_anomalies,
                                                         int *out_score,
                                                         int *out_flagged);

    int banana_core_anticheat_submit_driver_telemetry(const char *session_id,
                                                      uint32_t heartbeat_sequence,
                                                      int suspicious_ring12_driver_count,
                                                      int unsigned_driver_count,
                                                      int hidden_module_count,
                                                      int *out_score,
                                                      int *out_flagged);

    int banana_core_anticheat_get_session_status(const char *session_id,
                                                 int *out_score,
                                                 int *out_flagged,
                                                 uint32_t *out_integrity_hash);

#ifdef __cplusplus
}
#endif

#endif
