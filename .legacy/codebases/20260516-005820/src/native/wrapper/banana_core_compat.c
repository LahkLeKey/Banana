#include "banana_core.h"

#include "banana_wrapper.h"
#include "../calc/domain/services/calc_service.h"
#include "../batch/domain/services/batch_service.h"
#include "../ripeness/domain/services/ripeness_service.h"
#include "../anticheat/domain/entities/anticheat_session.h"
#include "../anticheat/domain/services/anticheat_scoring.h"
#include "../anticheat/domain/services/anticheat_trust.h"

#include <stdlib.h>
#include <string.h>

static int map_domain_rc(int rc)
{
    if (rc == 0)
        return BANANA_OK;
    if (rc == -2)
        return BANANA_NOT_FOUND;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_calculate(int purchases, int multiplier, int *out_total, int *out_base,
                          int *out_bonus)
{
    int rc = calc_calculate(purchases, multiplier, out_total, out_base, out_bonus);
    if (rc == 0)
        return BANANA_OK;
    if (rc == -2)
        return BANANA_OVERFLOW;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_predict_ripeness(const char *input_json, char **out_json)
{
    return map_domain_rc(ripeness_predict(input_json, out_json));
}

int banana_core_create_batch(const char *input_json, char **out_json)
{
    return map_domain_rc(batch_create(input_json, out_json));
}

int banana_core_get_batch_status(const char *batch_id, char **out_json)
{
    return map_domain_rc(batch_get_status(batch_id, out_json));
}

int banana_core_predict_batch_ripeness(const char *batch_id, char **out_json)
{
    return map_domain_rc(batch_predict_ripeness(batch_id, out_json));
}

int banana_core_create_harvest_batch(const char *input_json, char **out_json)
{
    return map_domain_rc(harvest_create(input_json, out_json));
}

int banana_core_add_bunch_to_harvest_batch(const char *batch_id, const char *input_json,
                                           char **out_json)
{
    return map_domain_rc(harvest_add_bunch(batch_id, input_json, out_json));
}

int banana_core_get_harvest_batch_status(const char *batch_id, char **out_json)
{
    return map_domain_rc(harvest_get_status(batch_id, out_json));
}

int banana_core_register_truck(const char *input_json, char **out_json)
{
    (void)input_json;
    (void)out_json;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_load_truck_container(const char *truck_id, const char *input_json, char **out_json)
{
    (void)truck_id;
    (void)input_json;
    (void)out_json;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_unload_truck_container(const char *truck_id, const char *container_id,
                                       char **out_json)
{
    (void)truck_id;
    (void)container_id;
    (void)out_json;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_relocate_truck(const char *truck_id, const char *input_json, char **out_json)
{
    (void)truck_id;
    (void)input_json;
    (void)out_json;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_get_truck_status(const char *truck_id, char **out_json)
{
    (void)truck_id;
    (void)out_json;
    return BANANA_INVALID_ARGUMENT;
}

int banana_core_anticheat_reset_session(const char *session_id)
{
    BananaAnticheatSession *session;

    if (!session_id)
        return BANANA_INVALID_ARGUMENT;

    session = anticheat_session_find(session_id, 1);
    if (!session)
        return BANANA_INTERNAL_ERROR;

    session->score = 0;
    session->confidence_score = 0;
    session->trust_state = BANANA_TRUST_NEW;
    session->integrity_hash = 0u;
    return BANANA_OK;
}

int banana_core_anticheat_submit_usermode_heartbeat(const char *session_id,
                                                     uint32_t heartbeat_sequence,
                                                     uint32_t simulation_tick,
                                                     int debugger_present,
                                                     int code_integrity_violations,
                                                     int memory_map_anomalies,
                                                     int timing_anomalies, int *out_score,
                                                     int *out_flagged)
{
    int risk_delta;
    BananaRiskConfidence rc;
    BananaAnticheatSession *session;

    if (!session_id || !out_score || !out_flagged)
        return BANANA_INVALID_ARGUMENT;

    risk_delta = debugger_present + code_integrity_violations + memory_map_anomalies +
                 timing_anomalies;
    anticheat_scoring_update(session_id, risk_delta, 0);
    anticheat_trust_update(session_id, risk_delta > 0 ? 3 : 1);

    session = anticheat_session_find(session_id, 1);
    if (!session)
        return BANANA_INTERNAL_ERROR;
    session->integrity_hash ^= heartbeat_sequence;
    session->integrity_hash ^= (simulation_tick << 1);

    rc = anticheat_scoring_get(session_id);
    *out_score = rc.risk_score;
    *out_flagged = (rc.risk_score >= 10 || session->trust_state >= BANANA_TRUST_RESTRICTED) ? 1 : 0;
    return BANANA_OK;
}

int banana_core_anticheat_submit_driver_telemetry(const char *session_id,
                                                   uint32_t heartbeat_sequence,
                                                   int suspicious_ring12_driver_count,
                                                   int unsigned_driver_count,
                                                   int hidden_module_count, int *out_score,
                                                   int *out_flagged)
{
    int risk_delta;
    BananaRiskConfidence rc;
    BananaAnticheatSession *session;

    if (!session_id || !out_score || !out_flagged)
        return BANANA_INVALID_ARGUMENT;

    risk_delta = suspicious_ring12_driver_count + unsigned_driver_count + hidden_module_count;
    anticheat_scoring_update(session_id, risk_delta, 0);
    anticheat_trust_update(session_id, risk_delta > 0 ? 5 : 1);

    session = anticheat_session_find(session_id, 1);
    if (!session)
        return BANANA_INTERNAL_ERROR;
    session->integrity_hash ^= (heartbeat_sequence << 2);

    rc = anticheat_scoring_get(session_id);
    *out_score = rc.risk_score;
    *out_flagged = (rc.risk_score >= 10 || session->trust_state >= BANANA_TRUST_RESTRICTED) ? 1 : 0;
    return BANANA_OK;
}

int banana_core_anticheat_get_session_status(const char *session_id, int *out_score,
                                             int *out_flagged, uint32_t *out_integrity_hash)
{
    BananaRiskConfidence rc;
    BananaAnticheatSession *session;

    if (!session_id || !out_score || !out_flagged || !out_integrity_hash)
        return BANANA_INVALID_ARGUMENT;

    session = anticheat_session_find(session_id, 0);
    if (!session)
        return BANANA_NOT_FOUND;

    rc = anticheat_scoring_get(session_id);
    *out_score = rc.risk_score;
    *out_flagged = (rc.risk_score >= 10 || session->trust_state >= BANANA_TRUST_RESTRICTED) ? 1 : 0;
    *out_integrity_hash = session->integrity_hash;
    return BANANA_OK;
}